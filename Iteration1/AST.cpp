#include <algorithm>
#include <vector>
#include <string>
#include <stack>
#include "PKB.h"
#include "Node.h"
#include "ExpressionTree.h"
#include "AST.h"
#include <regex>

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
	std:vector<Node*> mainProg;
	std::stack<std::string> bracesStack;	//this is to push "{" into the stack to keep track if the number of closing braces match
	std::vector<Node*> familyVector;	//this is to store node* in the order  0=procedure 1-...=while,if,else
	std::vector<Node*> stmLstParentVector;	//this is to store current statementlists of the procedure, while and ifelse
	int bracesNo = -1;
	
	for(int i = 0; i < sourceVector.size(); i++){
			line = sourceVector[i];
			int statementType = getStatementType(line);
			if(statementType == PROCEDURESTM && bracesStack.empty()){
				currentProcName = extractStatementPart(PROCEDURESTM, line);
				Node* procStm = pkb->createProcedure(currentProcName);
				Node* stmLst = pkb->createNode(statementList, i + 1);
				procStm->setLeftChild(stmLst);
				mainProg.push_back(procStm);

				bracesStack.push("{");
				familyVector.push_back(procStm);
				stmLstParentVector.push_back(stmLst);

			}else if(statementType == PROCEDURESTM && !bracesStack.empty()){
				cout<<"----PROCEDURE WITHIN A PROCEDURE----";
			}else{
				if(statementType == WHILESTM){
					bracesStack.push("{");

					std::string varName = extractStatementPart(WHILESTM, line);

					Node* whileStm = pkb->createNode(whileLoop, i + 1);
					

					Node* whileVar = pkb->createNode(variable, i + 1, varName, nullptr, nullptr, familyVector.front());//leftnode
					whileStm->setLeftChild(whileVar);
					whileVar->setRoot(familyVector.front());
					whileVar->setParent(familyVector[familyVector.size() - 1]);

					Node* whileStmLst = pkb->createNode(statementList, i + 1);	//rightnode
					

					whileStm->setRightChild(whileStmLst);
					whileStmLst->setRoot(familyVector.front());
					whileStmLst->setParent(familyVector[familyVector.size() - 1]);

					familyVector.push_back(whileStm);	
					stmLstParentVector[stmLstParentVector.size() - 1]->addStmt(whileStmLst);

				}else if(getStatementType(line) == CALLSTM){
					std::string callProcName = extractStatementPart(CALLSTM, line);

					if(callProcName.compare(currentProcName) == 0){
						cout<<"----RECURSIVE CALL NOT ALLOWED----";
					}

					Node* tCall = pkb->createNode(call, i + 1, callProcName);

					tCall->setParent(familyVector[familyVector.size() - 1]);
					tCall->setRoot(familyVector.front());

					stmLstParentVector[stmLstParentVector.size() - 1]->addStmt(tCall);

				}else if(statementType == ASSIGNSTM){
						Node* assignStm = pkb->createNode(assignment, i + 1);

						Node* assignVar = pkb->createNode(variable, i + 1, extractStatementPart(ASSIGNSTMVAR, line), 
							nullptr, assignStm, nullptr, familyVector.front());
						assignStm->setLeftChild(assignVar);
						assignVar->setRoot(familyVector.front());
						assignVar->setParent(familyVector[familyVector.size() - 1]);

						
						std::string inflix = extractStatementPart(ASSIGNSTMEXP, line);
						std::vector<char> postflix = expTree->expressionConverter(inflix);
						
						Node* parentNode = NULL;
						if(familyVector.size() > 1){// since procNode is inside the list, it is to prevent it from being set as parent of other nodes
							parentNode = familyVector[familyVector.size() - 1];
						}
						Node* assignExp = expTree->exptreeSetup(postflix, i + 1, assignStm, familyVector.front(), parentNode);

						assignStm->setRightChild(assignExp);
						assignExp->setRoot(familyVector.front());
						assignExp->setParent(familyVector[familyVector.size() - 1]);
						
						
				}
				
			}

			if(bracesNo = getNumOfClosingbraces(line)){
				if(bracesNo > bracesStack.size()){
					cout<<"ERROR IN SOURCE CODE, TOO MANY CLOSING BRACES";
				}
				while(bracesNo > 0){
					bracesStack.pop();
					familyVector.erase(familyVector.end() - 1);
					stmLstParentVector.erase(stmLstParentVector.end() - 1);
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
	regex procedure("procedure\s+\w+\s*{$");
	regex callProc("call\s+(\w+);}*$");
	regex whileLoop("while\s+\w*\s*{$");
	regex assignStm("\w+=[A-z0-9*+-\s]+;}*$");

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
	regex procedureName("procedure\s+(\w+)\s*{$");
	regex callProcName("call\s+(\w+);}*$");
	regex whileLoopVar("while\s+(\w*)\s*{$");
	regex assignStmLeftHand("(\w+)=[A-z0-9*+-]+;}*$");
	regex assignStmRightHand("(\w+=([A-z0-9*+-\s]+);}*$");
	smatch match;
	
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



