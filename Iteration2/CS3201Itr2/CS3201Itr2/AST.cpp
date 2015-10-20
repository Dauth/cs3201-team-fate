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

const int EMPTY = -1;
const int PROCEDURESTM = 1;
const int WHILESTM = 2;
const int IFTHENSTM = 3;
const int ELSESTM = 4;
const int CALLSTM = 5;
const int ASSIGNSTM = 6;
const int ASSIGNSTMVAR = 6;
const int ASSIGNSTMEXP = 7;
const char OPENBRACE = '{';
const char CLOSEDBRACE = '}';
const char SEMICOLON = ';';
const char EQUAL = '=';

using namespace std;


AST::AST(PKB* p, ExpressionTree* e){
	pkb = p;
	expTree = e;
}

void AST::catchParamProcException(std::string line, unsigned i){
	try{
		if(isParamProcedure(line)){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"PARAM PROCEDURE DETECTED AT LINE NO:"<<e<<std::endl;
		std::terminate();
	}
}

void AST::isCharABrace(bool& isBraces, char cChar)
{
	if(cChar == '{' || cChar == '}'){
		isBraces = true;
	}
}

void AST::catchMissingStmNameException(std::string& stmString, std::string& stmNameString, unsigned i)
{
	try{
		if(stmNameString.empty()){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"MISSING NAME FOR "<<stmString<<" AT LINE NO "<<e<<std::endl;
		std::terminate();
	}
}

void AST::catchIllegalOpenBraceException(unsigned i)
{
	std::cout<<"ILLEGAL OPEN BRACES OPERATION FOR CALL/ASSIGN STMT:"<<i + 1<<std::endl;
	std::terminate();
}

bool AST::isEqualSignPresent(std::string line)
{
	bool result = false;
	std::string::size_type n = line.find('=');
	if (n != std::string::npos){
		result = true;
	}
	return result;
}

void AST::getStatementTypeNew(std::string& stmString, int& statementType, char cChar, bool isSpace)
{
	if(statementType == EMPTY && 
		(!isSpace)){
			if(stmString.compare("procedure") == 0){
				statementType = PROCEDURESTM;
			}else if(stmString.compare("call") == 0){
				statementType = CALLSTM;
			}else if(stmString.compare("if") == 0){
				statementType = IFTHENSTM;
			}else if(stmString.compare("else") == 0){
				statementType = ELSESTM;
			}else if(stmString.compare("while") == 0){
				statementType = WHILESTM;
			}
	}
	if(statementType == EMPTY && cChar == SEMICOLON){
		if(isEqualSignPresent(stmString)){
			statementType = ASSIGNSTM;
		}
	}
}

void AST::appendStmTokens(std::string& stmString, std::string& stmNameString, int statementType, bool isBraces, char cChar, bool isSpace, bool isSemiColon)
{
	if(!isSpace && statementType == EMPTY && !isBraces && !isSemiColon){
		stmString.push_back(cChar);
	}else if(!isSpace && statementType != EMPTY && !isBraces && !isSemiColon){
		stmNameString.push_back(cChar);
	}
}

void AST::isCharAColon(bool& isSemiColon, char cChar)
{
	if(cChar == SEMICOLON){
		isSemiColon = true;
	}
}


void AST::clearStmHolder(std::string& stmString, std::string& stmNameString, int& statementType){
	stmString.clear();
	stmNameString.clear();
	statementType = -1;
}

bool AST::getIndexOfThen(std::string& stmNameString, int& indexOfThen){
	bool result = false;
	std::string::size_type n = stmNameString.find("then");
	if(n < std::string::npos){
		result = true;
		indexOfThen = n;
	}
	return result;
}

void AST::catchThenException(unsigned i, bool isThenPresent){
	try{
		if(!isThenPresent){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"MISSING THEN FOR IF STM AT LINE NO:"<<e<<std::endl;
		std::terminate();
	}
}

void AST::catchElseBeforeIfStmException(unsigned i, SyntType prevSynType){
	try{
		if(prevSynType != SyntType::ifelse){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"IF STM MUST PRECEDE ELSE STM AT LINE NO:"<<e<<std::endl;
		std::terminate();
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
	std::string stmString = "";
	std::string stmNameString = "";
	int statementType = -1;

	for(unsigned int i = 0; i < sourceVector.size(); i++){

		line = sourceVector[i];
		for(int j = 0; j < line.length(); j++){
			bool isBraces = false;
			bool isSemiColon = false;
			char cChar = line[j];
			bool isSpace = isspace(cChar);


			isCharABrace(isBraces, cChar);
			isCharAColon(isSemiColon, cChar);
			appendStmTokens(stmString, stmNameString, statementType, 
				isBraces, cChar, isSpace, isSemiColon);
			getStatementTypeNew(stmString, statementType, cChar, isSpace);


			if(isSemiColon){
				if(statementType == CALLSTM){
					catchMissingStmNameException(stmString, stmNameString, i);
					expTree->catchInvalidNameException(stmNameString);
					createCallNode(stmNameString, lineNumber, currentProcName, twinVector, i);
					lineNumber += 1;
					clearStmHolder(stmString, stmNameString, statementType);
				}else if(statementType == ASSIGNSTM){//for assignstm, all will be stored only in stmstring
					//catchMissingSemiColonException(line, i);
					createAssignNode(stmString, lineNumber, twinVector, i);
					lineNumber += 1;
					clearStmHolder(stmString, stmNameString, statementType);
				}
			}
			if(cChar == OPENBRACE){
				
				if(statementType >= 1 && statementType <= 3){	//when type is obtained, stmString is completed.
					catchMissingStmNameException(stmString, stmNameString, i);
					bracesStack.push("{");
					//fire event for while, proc and ifelse
					expTree->catchInvalidNameException(stmNameString);
					catchProcedureInceptionException(twinVector, i, statementType);
					if(statementType == PROCEDURESTM && twinVector.empty()){
						catchParamProcException(stmNameString, i);
						currentProcName = stmNameString;
						createProcNode(stmNameString, mainProg, bracesStack, twinVector, i);
						clearStmHolder(stmString, stmNameString, statementType);
					}else if(statementType == WHILESTM){
						createWhileNode(stmNameString, lineNumber, twinVector);
						lineNumber += 1;
						clearStmHolder(stmString, stmNameString, statementType);
					}else if(statementType == IFTHENSTM){
						int indexOfThen = -1;
						bool isThenPresent = getIndexOfThen(stmNameString, indexOfThen);
						catchThenException(i, isThenPresent);
						std::string ifStmName = stmNameString.substr(0, indexOfThen);
						createIfElseNode(ifStmName, lineNumber, twinVector);
						lineNumber += 1;
						clearStmHolder(stmString, stmNameString, statementType);
					}
				}else if(statementType == ELSESTM && stmNameString.empty()){
					bracesStack.push("{");
					SyntType prevSynType = twinVector[twinVector.size() - 1]->getStmNode()->getType();
					catchElseBeforeIfStmException(i, prevSynType);
					lineNumber += 1;
					clearStmHolder(stmString, stmNameString, statementType);
				}else{
					//throw exception here
					//openbraces follows only parent stm such as ifelse, while, if else
					catchIllegalOpenBraceException(i);
				}
			}
			if(cChar == CLOSEDBRACE){
				catchUnequalBracesException(bracesStack, i);
				catchEmptyContainerException(twinVector, i);
				popImmediateParent(bracesStack, twinVector);
			}
		}





	}

	return mainProg;

}

void AST::catchProcedureInceptionException(std::vector<Twin*>twinVector, unsigned i, int statementType){
	try{
		if(statementType == PROCEDURESTM && !twinVector.empty()){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"PROCEDURE WITHIN A PROCEDURE (INCEPTION) AT LINE NO:"<<e<<std::endl;
		std::terminate();
	}
}

void AST::catchDupProcedureException(unsigned i, Node* procStm){
	try{
		if(procStm == nullptr){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"DUPLICATED PROCEDURE APPEARED IN PROGRAM AT LINE NO:"<<e<<std::endl;
		std::terminate();
	}
}

void AST::catchRecursiveCallException(std::string& currentProcName, unsigned i, std::string& callProcName){
	try{
		if(callProcName.compare(currentProcName) == 0){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"RECURSIVE CALL DETECTED AT LINE NO:"<<e<<std::endl;
		std::terminate();
	}
}

void AST::catchUnbalancedInfixException(unsigned i, bool isInflixBalance){
	try{
		if(!isInflixBalance){
			throw i + 1;
		}
	}catch (unsigned int e){
		std::cout<<"INFLIX EXPRESION IS NOT BALANCED AT LINE NO:"<<e<<std::endl;
		std::terminate();
	}
}

void AST::catchUnequalBracesException(std::stack<std::string>& bracesStack, unsigned i){
	try{
		if(bracesStack.empty()){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"ERROR IN SOURCE CODE, TOO MANY CLOSING BRACES FROM LINE NO:"<<e<<std::endl;
		std::terminate();
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
		std::terminate();
	}
}

bool AST::isParamProcedure(std::string input){
	bool result = false;
	std::string::size_type n = -1;
	std::string::size_type m = -1;
	n = input.find("(");
	m = input.find(")");
	if (n != std::string::npos || m != std::string::npos){
		result = true;
	}
	return result;
}

void AST::createIfElseNode(std::string varName, int lineNumber, std::vector<Twin*>& twinVector){

	Node* ifStm = nullptr;

	Node* procNode = twinVector.front()->getStmNode();

	Node* thenStmLst = nullptr;
	Node* elseStmLst = nullptr;

	setupIfThenListNode(lineNumber, twinVector, varName, ifStm, procNode, thenStmLst, elseStmLst);

	Twin* tTwin = new Twin(ifStm, thenStmLst);
	Twin* yTwin = new Twin(ifStm, elseStmLst);

	twinVector[twinVector.size() - 1]->getStmListNode()->addStmt(ifStm);

	twinVector.push_back(yTwin);
	twinVector.push_back(tTwin);
}

void AST::setupIfThenListNode(int lineNumber, std::vector<Twin*>& twinVector, std::string varName, Node*& ifStm, Node* procNode, Node*& thenStmLst, Node*& elseStmLst){
	Node* ifVar = nullptr;
	Node* parentNode = nullptr;
	if(twinVector.size() > 1){// 1 proc statementlist and 1 or more while statementlist
		parentNode = twinVector[twinVector.size() - 1]->getStmNode();						
		ifStm = pkb->createNode(ifelse, lineNumber, "", nullptr, nullptr, parentNode, procNode);
		ifVar = pkb->createNode(variable, lineNumber, varName, ifStm, nullptr, parentNode, procNode);//leftnode
	}else if(twinVector.size() == 1){// only a procedure statementlist in it
		ifStm = pkb->createNode(ifelse, lineNumber, "", nullptr, nullptr, nullptr, procNode);
		ifVar = pkb->createNode(variable, lineNumber, varName, ifStm, nullptr, nullptr, procNode);//leftnode
	}

	ifStm->setLeftChild(ifVar);

	thenStmLst = pkb->createNode(statementList, lineNumber);
	elseStmLst = pkb->createNode(statementList, lineNumber);
	//rightnode
	ifStm->setRightChild(thenStmLst);
	ifStm->setThirdChild(elseStmLst);
}

void AST::createWhileNode(std::string varName, int lineNumber, std::vector<Twin*>& twinVector){

	Node* whileStm = nullptr;

	Node* procNode = twinVector.front()->getStmNode();

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

void AST::extractExpressionVar(std::string& line, std::vector<std::string>& assignVector){
	int cIndex = 0;
	char tChar;
	std::string appendString = "";
	for(int i = 0; i < line.length(); i++){
		tChar = line[i];
		if (tChar != EQUAL){
			appendString.push_back(tChar);
		}else if(tChar == EQUAL){
			assignVector.push_back(appendString);
			appendString.clear();
			cIndex += 1;
		}
	}
	if(!appendString.empty()){
		assignVector.push_back(appendString);
	}
}

void AST::catchExpressionException(unsigned i, std::vector<std::string>& assignVector){
	try{
		if(assignVector.size() != 2){
			throw i + 1;
		}
	}catch(unsigned int e){
		std::cout<<"ERROR WITH EXPRESSION AT LINE NO:"<<e<<std::endl;
		std::terminate();
	}
}

void AST::createAssignNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector, unsigned i){
	std::vector<std::string> assignVector;// should have size of 2 only. Left and Right side without the equal
	extractExpressionVar(line, assignVector);
	catchExpressionException(i, assignVector);


	std::string varName = assignVector[0];
	expTree->catchInvalidNameException(varName);

	Node* procNode = twinVector.front()->getStmNode();
	Node* assignStm = nullptr;

	setupAssignVarListNode(assignVector[1], lineNumber, twinVector, i, varName, procNode, assignStm);

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

	bool isInflixBalance = expTree->isInflixBalanced(line);

	catchUnbalancedInfixException(i, isInflixBalance);

	std::vector<std::string> postflix = expTree->expressionConverter(line);

	Node* assignExp = expTree->exptreeSetup(pkb, postflix, lineNumber, assignStm, procNode, parentNode);

	assignStm->setRightChild(assignExp);
}

void AST::popImmediateParent(std::stack<std::string>& bracesStack, std::vector<Twin*>& twinVector){
	bracesStack.pop();
	twinVector.erase(twinVector.end() - 1);
}

void AST::createCallNode(std::string stmName, int lineNumber, std::string& currentProcName, std::vector<Twin*>& twinVector, unsigned i){
	catchRecursiveCallException(currentProcName, i, stmName);

	Node* tCall = nullptr;
	Node* procNode = twinVector.front()->getStmNode();

	if(twinVector.size() > 1){// 1 proc statementlist and 1 or more while statementlist
		Node* parentNode = twinVector[twinVector.size() - 1]->getStmNode();						
		tCall = pkb->createNode(call, lineNumber, stmName, nullptr, nullptr, parentNode, procNode);
	}else if(twinVector.size() == 1){// only a procedure statementlist in it
		tCall = pkb->createNode(call, lineNumber, stmName, nullptr, nullptr, nullptr, procNode);
	}

	twinVector[twinVector.size() - 1]->getStmListNode()->addStmt(tCall);
}

void AST::createProcNode(std::string& currentProcName, std::vector<Node*>& mainProg, std::stack<std::string>& bracesStack, std::vector<Twin*>& twinVector, unsigned i){

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