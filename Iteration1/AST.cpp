#include "stdafx.h"
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
#include <iostream>

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
}


void AST::catchMissingSemiColonException(std::string line, unsigned i){
	try{
		if(!isSemiColonPresent(line)){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"MISSING SEMICOLON FOR ASSIGN STMT AT LINE NO:"<<e<<std::endl;
	}
}

/*
This function processes a vector which contains the source file
Parameters: vector<string>
Return:		vector<Node*>
*/
std::vector<Node*> AST::buildAST(std::vector<std::string> sourceVector){
	std::string line;
	int lineNumber = 1;
	std::string currentProcName;
	std::vector<Node*> mainProg;
	std::stack<std::string> bracesStack;	//this is to push "{" into the stack to keep track if the number of closing braces match
	std::vector<Twin*> twinVector;//Twin store as   <stmNode, stmListNode>, at any point of time, there can only be 1 proc in it

	for(unsigned int i = 0; i < sourceVector.size(); i++){
		unsigned int closeBracesNo = -1;
		unsigned int openBracesNo = -1;
		line = sourceVector[i];
		//std::cout << lineNumber << "." << line << "\n";

		int statementType = getStatementType(line);

		catchProcedureInceptionException(bracesStack, i, statementType);

		if(statementType == PROCEDURESTM && bracesStack.empty()){
			createProcNode(line, currentProcName, mainProg, bracesStack, twinVector, i);
		}else{
			if(statementType == WHILESTM){
				//bracesStack.push("{");
				createWhileNode(line, lineNumber, twinVector);
				lineNumber += 1;

			}else if(statementType == CALLSTM){
				//need to check for semicolon here in future
				createCallNode(line, lineNumber, currentProcName, twinVector, i);
				lineNumber += 1;
			}else if(statementType == ASSIGNSTM){
				catchMissingSemiColonException(line, i);
				createAssignNode(line, lineNumber, twinVector, i);
				lineNumber += 1;
			}
		}

		openBracesNo = getNumOfOpenbraces(line);
		closeBracesNo = getNumOfClosingbraces(line);

		if(openBracesNo != -1){
			while(openBracesNo > 0){
				bracesStack.push("{");
				openBracesNo--;
			}
		}

		if(closeBracesNo != -1){
			catchUnequalBracesException(bracesStack, closeBracesNo, i);
			while(closeBracesNo > 0){
				catchEmptyContainerException(twinVector, i);
				popImmediateParent(bracesStack, twinVector, closeBracesNo);
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
	std::regex procedure("\\s*\\t*procedure\\s+[A-Za-z]+\\s*\\{$");
	std::regex callProc("\\s*\\t*call\\s+[A-Za-z]+\\;\\s*\\}*$");
	std::regex whileLoop("\\s*\\t*while\\s+[A-Za-z]+\\s*\\{$");
	std::regex assignStm("\\s*\\t*[A-Za-z]+\\s*=[A-Za-z0-9\\*\\+\\-\\s\\(\\)]+\\;?\\s*\\}*$");
	std::smatch match;
	int result = -1;

	if (std::regex_search(input, match, procedure)){
		result = PROCEDURESTM;
	}else if(std::regex_search(input, match, callProc)){
		result = CALLSTM;
	}else if(std::regex_search(input, match, whileLoop)){
		result = WHILESTM;	
	}else if (std::regex_search(input, match, assignStm)){
		result = ASSIGNSTM;
	}


	return result;
}

/*
This function returns a string without white spaces which can be one of the following:procedure's name, name of procedure to be called, 
control variable of the while loop, variable of assignment, expresion of assignment
Parameters: int, string
Return:		string
*/
std::string AST::extractStatementPart(int inputType, std::string input){
	std::regex procedureName("\\s*\\t*procedure\\s+([A-Za-z]+)\\s*\\{$");
	std::regex callProcName("\\s*\\t*call\\s+([A-Za-z]+)\\;\\s*\\}*$");
	std::regex whileLoopVar("\\s*\\t*while\\s+([A-Za-z]+)\\s*\\{$");
	std::regex assignStmLeftHand("\\s*\\t*([A-Za-z]+)\\s*=[A-Za-z0-9\\*\\+\\-\\s\\(\\)]+\\;?\\s*\\}*$");
	std::regex assignStmRightHand("\\s*\\t*[A-Za-z]+\\s*=([A-Za-z0-9\\*\\+\\-\\s\\(\\)]+)\\;?\\s*\\}*$");
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
  
  if(match.length() > 0){
    outcome = match[1];
    outcome.erase(std::remove(outcome.begin(), outcome.end(), ' '),outcome.end());
  }
  
	return outcome;
}

/*
This function returns an int which contains the details of the number of closing braces in a line
Parameters: string
Return:		int
*/
int AST::getNumOfClosingbraces(std::string input){
	std::regex closingBraces("(\\}+)$");
	std::smatch match;

	int result = -1;

	std::regex_search(input, match, closingBraces);
	if(match.length() > 0){
        result = match[1].length();
    }
	return result;
}

int AST::getNumOfOpenbraces(std::string input){
	std::regex openBraces("(\\{+)");
	std::smatch match;

	int result = -1;

	std::regex_search(input, match, openBraces);
	if(match.length() > 0){
        result = match[1].length();
    }
	return result;
}

bool AST::isSemiColonPresent(std::string input){
	std::regex openBraces("(\\;+)");
	std::smatch match;

	int result = false;

	std::regex_search(input, match, openBraces);
	if(match.length() > 0){
        result = true;
    }
	return result;
}

void AST::catchProcedureInceptionException(std::stack<std::string>& bracesStack, unsigned i, int statementType){
	try{
		if(statementType == PROCEDURESTM && !bracesStack.empty()){
			throw i + 1;
		}
	}catch(unsigned int e){
			std::cout<<"PROCEDURE WITHIN A PROCEDURE (INCEPTION) AT LINE NO:"<<e<<std::endl;
	}
}

void AST::catchDupProcedureException(unsigned i, Node* procStm){
	try{
		if(procStm == nullptr){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"DUPLICATED PROCEDURE APPEARED IN PROGRAM AT LINE NO:"<<e<<std::endl;
	}
}

void AST::catchRecursiveCallException(std::string& currentProcName, unsigned i, std::string& callProcName){
	try{
		if(callProcName.compare(currentProcName) == 0){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"RECURSIVE CALL DETECTED AT LINE NO:"<<e<<std::endl;
	}
}

void AST::catchUnbalancedInfixException(unsigned i, bool isInflixBalance){
	try{
		if(!isInflixBalance){
			throw i + 1;
		}
	}catch (unsigned int e){
		std::cout<<"INFLIX EXPRESION IS NOT BALANCED AT LINE NO:"<<e<<std::endl;
	}
}

void AST::catchUnequalBracesException(std::stack<std::string>& bracesStack, unsigned bracesNo, unsigned i){
	try{
		if(bracesNo > bracesStack.size()){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"ERROR IN SOURCE CODE, TOO MANY CLOSING BRACES FROM LINE NO:"<<e<<std::endl;
	}
}

void AST::catchEmptyContainerException(std::vector<Twin*>& twinVector, unsigned i){
	try{
		int test = twinVector[twinVector.size() - 1]->getStmListNode()->getStmtLst().size();
		if(test == 0){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"EMPTY STATEMENT IN CONTAINER AT LINE NO:"<<e<<std::endl;
	}
}

void AST::createWhileNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector){
	std::string varName = extractStatementPart(WHILESTM, line);

	Node* whileStm = nullptr;

	Node* procNode = procNode = twinVector.front()->getStmNode();

	Node* whileStmLst = nullptr;
	setupWhileVarListNode(lineNumber, twinVector, varName, whileStm, procNode, whileStmLst);

	Twin* tTwin = new Twin(whileStm, whileStmLst);

	twinVector[twinVector.size() - 1]->getStmListNode()->addStmt(whileStm);

	twinVector.push_back(tTwin);
}

void AST::setupWhileVarListNode(int lineNumber, std::vector<Twin*>& twinVector, std::string varName, Node*& whileStm, Node* procNode, Node*& whileStmLst){
	Node* whileVar = nullptr;
	Node* parentNode = nullptr;
	if(twinVector.size() > 1){// 1 proc statementlist and 1 or more while statementlist
		parentNode = twinVector[twinVector.size() - 1]->getStmNode();						
		whileStm = pkb->createNode(whileLoop, lineNumber, "", nullptr, nullptr, parentNode, procNode);
		whileVar = pkb->createNode(variable, lineNumber, varName, whileStm, nullptr, parentNode, procNode);//leftnode
	}else if(twinVector.size() == 1){// only a procedure statementlist in it
		whileStm = pkb->createNode(whileLoop, lineNumber, "", nullptr, nullptr, nullptr, procNode);
		whileVar = pkb->createNode(variable, lineNumber, varName, whileStm, nullptr, nullptr, procNode);//leftnode
	}
							
	whileStm->setLeftChild(whileVar);

	whileStmLst = pkb->createNode(statementList, lineNumber);
	//rightnode
	whileStm->setRightChild(whileStmLst);
}

void AST::createAssignNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector, unsigned i){
	std::string varName = extractStatementPart(ASSIGNSTMVAR, line);

	Node* procNode = twinVector.front()->getStmNode();
	Node* assignStm = nullptr;

	setupAssignVarListNode(line, lineNumber, twinVector, i, varName, procNode, assignStm);

	twinVector[twinVector.size() - 1]->getStmListNode()->addStmt(assignStm);
}

void AST::setupAssignVarListNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector, unsigned i, std::string varName, Node* procNode, Node*& assignStm){

	Node* parentNode = nullptr;
	Node* assignVar = nullptr;

	if(twinVector.size() > 1){// 1 proc statementlist and 1 or more while statementlist
		parentNode = twinVector[twinVector.size() - 1]->getStmNode();						
		assignStm = pkb->createNode(assignment, lineNumber, "", nullptr, nullptr, parentNode, procNode);
		assignVar = pkb->createNode(variable, lineNumber, varName, 
		                            nullptr, assignStm, parentNode, procNode);
	}else if(twinVector.size() == 1){// only a procedure statementlist in it
		assignStm = pkb->createNode(assignment, lineNumber, "", nullptr, nullptr, nullptr, procNode);
		assignVar = pkb->createNode(variable, lineNumber, varName, 
		                            nullptr, assignStm, nullptr, procNode);
	}
					
	assignStm->setLeftChild(assignVar);
					
	std::string inflix = extractStatementPart(ASSIGNSTMEXP, line);
	bool isInflixBalance = expTree->isInflixBalanced(inflix);

	catchUnbalancedInfixException(i, isInflixBalance);

	std::vector<char> postflix = expTree->expressionConverter(inflix);
						
	Node* assignExp = expTree->exptreeSetup(postflix, lineNumber, assignStm, procNode, parentNode);

	assignStm->setRightChild(assignExp);
}

void AST::popImmediateParent(std::stack<std::string>& bracesStack, std::vector<Twin*>& twinVector, unsigned& bracesNo){
	bracesStack.pop();
	twinVector.erase(twinVector.end() - 1);
	bracesNo -= 1;
}

void AST::createCallNode(std::string line, int lineNumber, std::string& currentProcName, std::vector<Twin*>& twinVector, unsigned i){
	std::string callProcName = extractStatementPart(CALLSTM, line);

	catchRecursiveCallException(currentProcName, i, callProcName);

	Node* tCall = pkb->createNode(call,lineNumber, callProcName);

	twinVector[twinVector.size() - 1]->getStmListNode()->addStmt(tCall);
}

void AST::createProcNode(std::string line, std::string& currentProcName, std::vector<Node*>& mainProg, std::stack<std::string>& bracesStack, std::vector<Twin*>& twinVector, unsigned i){
	currentProcName = extractStatementPart(PROCEDURESTM, line);

	Node* procStm = nullptr;
	Node* stmLst = nullptr;
	
	procStm = pkb->createProcedure(currentProcName);

	catchDupProcedureException(i, procStm);

	stmLst = pkb->createNode(statementList, i + 1);
	procStm->setLeftChild(stmLst);

	mainProg.push_back(procStm);

	//bracesStack.push("{");

	Twin* tTwin = new Twin(procStm, stmLst);
	twinVector.push_back(tTwin);
}