#include <algorithm>
#include <vector>
#include <string>
#include <stack>
#include "PKB.h"
#include "Node.h"
#include "ExpressionTree.h"
#include "AST.h"
#include <regex>
#include "Twin.h"

const int PROCEDURESTM = 1;
const int CALLSTM = 2;
const int WHILESTM = 3;
const int ASSIGNSTM = 4;
const int ASSIGNSTMVAR = 4;
const int ASSIGNSTMEXP = 5;

using namespace std;


AST::AST(PKB* p, ExpressionTree* e){
	pkb = p;
	expTree = e;
};

/*
This function processes a vector which contains the source file
Parameters: vector<string>
Return:		vector<Node*>
*/
std::vector<Node*> AST::buildAST(std::vector<std::string> sourceVector){
	std::string line;
	std::string currentProcName;
	std::vector<Node*> mainProg;
	std::stack<std::string> bracesStack;	//this is to push "{" into the stack to keep track if the number of closing braces match
	std::vector<Twin*> twinVector;//Twin store as   <stmNode, stmListNode>
	unsigned int bracesNo = -1;
	
	for(unsigned int i = 0; i < sourceVector.size(); i++){
			line = sourceVector[i];
			int statementType = getStatementType(line);
			if(statementType == PROCEDURESTM && bracesStack.empty()){
				currentProcName = extractStatementPart(PROCEDURESTM, line);
				Node* procStm = pkb->createProcedure(currentProcName);
				Node* stmLst = pkb->createNode(statementList, i + 1);
				procStm->setLeftChild(stmLst);
				mainProg.push_back(procStm);

				bracesStack.push("{");

				Twin* tTwin = new Twin(procStm, stmLst);
				twinVector.push_back(tTwin);

			}else if(statementType == PROCEDURESTM && !bracesStack.empty()){
				//cout<<"----PROCEDURE WITHIN A PROCEDURE----";
			}else{
				if(statementType == WHILESTM){
					bracesStack.push("{");

					std::string varName = extractStatementPart(WHILESTM, line);

					Node* whileStm = nullptr;
					Node* whileVar = nullptr;

					Node* parentNode = nullptr;
					Node* procNode = procNode = twinVector.front()->getStmNode();

					if(twinVector.size() > 1){// 1 proc statementlist and 1 or more while statementlist
						parentNode = twinVector[twinVector.size() - 1]->getStmNode();						
						whileStm = pkb->createNode(whileLoop, i + 1, "", nullptr, nullptr, parentNode, procNode);
						whileVar = pkb->createNode(variable, i + 1, varName, nullptr, parentNode, procNode);//leftnode
					}else if(twinVector.size() == 1){// only a procedure statementlist in it
						whileStm = pkb->createNode(whileLoop, i + 1, "", nullptr, nullptr, nullptr, procNode);
						whileVar = pkb->createNode(variable, i + 1, varName, nullptr, nullptr, procNode);//leftnode
					}
							
					whileStm->setLeftChild(whileVar);

					Node* whileStmLst = pkb->createNode(statementList, i + 1);	//rightnode
					whileStm->setRightChild(whileStmLst);

					Twin* tTwin = new Twin(whileStm, whileStmLst);
					twinVector.push_back(tTwin);

				}else if(getStatementType(line) == CALLSTM){
					std::string callProcName = extractStatementPart(CALLSTM, line);

					if(callProcName.compare(currentProcName) == 0){
						//cout<<"----RECURSIVE CALL NOT ALLOWED----";
					}

					Node* tCall = pkb->createNode(call, i + 1, callProcName);

					twinVector[twinVector.size() - 1]->getStmListNode()->addStmt(tCall);

				}else if(statementType == ASSIGNSTM){
					std::string varName = extractStatementPart(ASSIGNSTMVAR, line);

					Node* parentNode = nullptr;
					Node* procNode = twinVector.front()->getStmNode();
					Node* assignStm = nullptr;
					Node* assignVar = nullptr;

					if(twinVector.size() > 1){// 1 proc statementlist and 1 or more while statementlist
						parentNode = twinVector[twinVector.size() - 1]->getStmNode();						
						assignStm = pkb->createNode(assignment, i + 1, "", nullptr, nullptr, parentNode, procNode);
						assignVar = pkb->createNode(variable, i + 1, varName, 
							nullptr, assignStm, parentNode, procNode);
					}else if(twinVector.size() == 1){// only a procedure statementlist in it
						assignStm = pkb->createNode(assignment, i + 1, "", nullptr, nullptr, nullptr, procNode);
						assignVar = pkb->createNode(variable, i + 1, varName, 
							nullptr, assignStm, nullptr, procNode);
					}

						assignStm->setLeftChild(assignVar);
						twinVector[twinVector.size() - 1]->getStmListNode()->addStmt(assignStm);
						
						std::string inflix = extractStatementPart(ASSIGNSTMEXP, line);
						std::vector<char> postflix = expTree->expressionConverter(inflix);
						
						Node* assignExp = expTree->exptreeSetup(postflix, i + 1, assignStm, procNode, parentNode);

						assignStm->setRightChild(assignExp);												
				}
				
			}

			if(bracesNo = getNumOfClosingbraces(line)){
				if(bracesNo > bracesStack.size()){
					//cout<<"ERROR IN SOURCE CODE, TOO MANY CLOSING BRACES";
				}
				while(bracesNo > 0){
					bracesStack.pop();
					twinVector.erase(twinVector.end() - 1);
				}
			}
	}
	return mainProg;
}

/*
This function returns an int that cross reference the const to check which statement it mataches. Example: procedure,call,while,assign
Parameters: string
Return:		int
*/
int AST::getStatementType(std::string input){
	std::regex procedure("procedure\\s+[A-z]+\\s*\\{$");
	std::regex callProc("call\\s+[A-z]+\\;\\}*$");
	std::regex whileLoop("while\\s+[A-z]+\\s*\\{$");
	std::regex assignStm("[A-z]+\\s*=[A-z0-9\\*\\+\\-\\s]+\\;\\}*$");
	
	int result = -1;

	if(regex_match(input, procedure)){
		result = PROCEDURESTM;
	}else if(regex_match(input, callProc)){
		result = CALLSTM;
	}else if(regex_match(input, whileLoop)){
		result = WHILESTM;	
	}else if(regex_match(input, assignStm)){
		result = ASSIGNSTM;	
	}
	return result;
}

/*
This function returns a string which can be one of the following:procedure's name, name of procedure to be called, 
control variable of the while loop, variable of assignment, expresion of assignment
Parameters: int, string
Return:		string
*/
std::string AST::extractStatementPart(int inputType, std::string input){
	std::regex procedureName("procedure\\s+([A-z])+\\s*\\{$");
	std::regex callProcName("call\\s+([A-z])+\\;\\}*$");
	std::regex whileLoopVar("while\\s+([A-z])+\\s*\\{$");
	std::regex assignStmLeftHand("([A-z])+\\s*=[A-z0-9\\*\\+\\-\\s]+\\;\\}*$");
	std::regex assignStmRightHand("[A-z]+\\s*=([A-z0-9\\*\\+\\-\\s])+\\;\\}*$");
	std::smatch match;
	
	std::string outcome = "";

	if(inputType == PROCEDURESTM){
		std::regex_search(input, match, procedureName);
	}else if(inputType == CALLSTM){
		std::regex_search(input, match, callProcName);
	}else if(inputType == WHILESTM){
		std::regex_search(input, match, whileLoopVar);
	}else if(inputType == ASSIGNSTMVAR){
		std::regex_search(input, match, assignStmLeftHand);
	}else if(inputType == ASSIGNSTMEXP){
		std::regex_search(input, match, assignStmRightHand);
	}

	return match[0];
}

/*
This function returns an int which contains the details of the number of closing braces in a line
Parameters: string
Return:		int
*/
int AST::getNumOfClosingbraces(std::string input){
	std::regex closingBraces(";}*$");
	std::smatch match;

	int result = -1;

	if(std::regex_search(input, match, closingBraces)){
		result = match[0].length();
	}

	return result;
}



