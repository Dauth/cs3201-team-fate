#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include "PKBFacade.h"
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


AST::AST(PKBFacade* p, ExpressionTree* e){
	pkb = p;
	expTree = e;
}

//This method is used to catch procedures with parameters
//PRE: source line and line number
//POST: none
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

//This method is used to check if a character is an opening or close brace
//PRE: boolean isBraces and character
//POST: none
void AST::isCharABrace(bool& isBraces, char cChar)
{
	if(cChar == '{' || cChar == '}'){
		isBraces = true;
	}
}

//This method is used to catch missing statement name
//PRE: statementstring, statementNamestring and line number
//POST: none
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

//This method is used to catch for open braces for call/assign
//PRE: linenumber
//POST: none
void AST::catchIllegalOpenBraceException(unsigned i)
{
	std::cout<<"ILLEGAL OPEN BRACES OPERATION FOR CALL/ASSIGN STMT:"<<i + 1<<std::endl;
	std::terminate();
}

//This method is used to check if an equal sign is present for assignment
//PRE: source line
//POST: none
bool AST::isEqualSignPresent(std::string line)
{
	bool result = false;
	std::string::size_type n = line.find('=');
	if (n != std::string::npos){
		result = true;
	}
	return result;
}


//This method is used to check the statement type of a string
//PRE: statement string, statementtype, current character, a boolean to see if current character is space or not,source line
//POST: none
void AST::getStatementTypeNew(std::string& stmString, int& statementType, char cChar, bool isSpace, std::string line)
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


//This method is used append characters to a string
//PRE: statement string, statement name string, statement type, boolean to check if current character is a brace or not, current character, 
//boolean to check if current character is a space, boolean to check if a current character is a semicolon
//POST: none
void AST::appendStmTokens(std::string& stmString, std::string& stmNameString, int statementType, bool isBraces, char cChar, bool isSpace, bool isSemiColon)
{
	if(!isSpace && statementType == EMPTY && !isBraces && !isSemiColon){
		stmString.push_back(cChar);
	}else if(!isSpace && statementType != EMPTY && !isBraces && !isSemiColon){
		stmNameString.push_back(cChar);
	}
}

//This method is used check if a character is a semi colon
//PRE: boolean pointer, character
//POST: none
void AST::isCharAColon(bool& isSemiColon, char cChar)
{
	if(cChar == SEMICOLON){
		isSemiColon = true;
	}
}

//This method is used to clear statement and statement name string
//PRE: statement string, statement name string, statement type
//POST: none
void AST::clearStmHolder(std::string& stmString, std::string& stmNameString, int& statementType){
	stmString.clear();
	stmNameString.clear();
	statementType = -1;
}

//This method is used to check for the index of 'then' in a string
//PRE: statement string, int pointer for index of then
//POST: none
bool AST::getIndexOfThen(std::string& stmNameString, int& indexOfThen){
	bool result = false;
	std::string::size_type n = stmNameString.find("then");
	if(n < std::string::npos){
		result = true;
		indexOfThen = n;
	}
	return result;
}


//This method is catch for missing 'then' in a if statement
//PRE: line number, boolean 
//POST: none
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

//This method is used to catch if an 'else' appear before an 'if'
//PRE: line number, SyntType
//POST: none
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
	std::string dotGraph = "digraph AST{\n graph [ordering=\"out\"];\n prog[label=\"MainProg\" shape=box];\n";
	std::vector<int> graphVector;
	int counter = 1;

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
			getStatementTypeNew(stmString, statementType, cChar, isSpace, line);


			if(isSemiColon){
				if(statementType == CALLSTM){
					catchMissingStmNameException(stmString, stmNameString, i);
					expTree->catchInvalidNameException(stmNameString);
					createCallNode(stmNameString, lineNumber, currentProcName, twinVector, i, dotGraph, counter, graphVector);
					lineNumber += 1;
					clearStmHolder(stmString, stmNameString, statementType);
				}else if(statementType == ASSIGNSTM){//for assignstm, all will be stored only in stmstring
					//catchMissingSemiColonException(line, i);
					createAssignNode(stmString, lineNumber, twinVector, i, dotGraph, counter, graphVector);
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
						createProcNode(stmNameString, mainProg, bracesStack, twinVector, i, dotGraph, counter, graphVector);
						clearStmHolder(stmString, stmNameString, statementType);
					}else if(statementType == WHILESTM){
						createWhileNode(stmNameString, lineNumber, twinVector, dotGraph, counter, graphVector);
						lineNumber += 1;
						clearStmHolder(stmString, stmNameString, statementType);
					}else if(statementType == IFTHENSTM){
						int indexOfThen = -1;
						bool isThenPresent = getIndexOfThen(stmNameString, indexOfThen);
						catchThenException(i, isThenPresent);
						std::string ifStmName = stmNameString.substr(0, indexOfThen);
						createIfElseNode(ifStmName, lineNumber, twinVector, dotGraph, counter, graphVector);
						lineNumber += 1;
						clearStmHolder(stmString, stmNameString, statementType);
					}
				}else if(statementType == ELSESTM && stmNameString.empty()){
					bracesStack.push("{");
					SyntType prevSynType = twinVector[twinVector.size() - 1]->getStmNode()->getType();
					catchElseBeforeIfStmException(i, prevSynType);
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
				graphVector.pop_back();
			}
		}





	}
	/*Graph drawing*/
	dotGraph.append("}");
	ofstream myfile;
	myfile.open ("ast.dot");
	myfile << dotGraph;
	myfile.close();
	////
	return mainProg;

}

//This method is used to catch procedures within a procedure
//PRE: vector of twin, line number, int of statement type
//POST: none
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

//This method is used to catch for duplicated procedures in a simple source
//PRE: line number, Node* 
//POST: none
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

//This method is used to catch for a procedure calling itself
//PRE: string pointer of current procedure name, line number, string name of the procedure to be string
//POST: none
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

//This method is used to catch if an infix expression is not balanced
//PRE: line number, boolean 
//POST: none
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

//This method is used to catch if there are either more opening or closing braces
//PRE: stack, line number
//POST: none
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

//This method is used to catch if there are no statements in a container
//PRE: vector of twin, line number
//POST: none
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

//This method is used to check if there is either a '(' or ')' in a procedure statement
//PRE: input string
//POST: none
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

//This method is used to create if else node
//PRE: variable name, line number, vector of twin, string pointer for graph drawing, int, vector of int for graph drawing
//POST: none
void AST::createIfElseNode(std::string varName, int lineNumber, std::vector<Twin*>& twinVector, std::string& dotGraph, int& counter, std::vector<int>& graphVector){

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


	/*Graph Drawing*/
	std::string ifStmString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() +"[label=\":if\" shape=box];\n";
	std::string ifStmVarString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() + "[label=\""+ varName +":variable\" shape=box];\n";
	std::string thenStmLstString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() + "[label=\"then:stmLst\" shape=box];\n";
	std::string elseStmLstString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() + "[label=\"else:stmLst\" shape=box];\n";
	dotGraph.append(ifStmString);
	dotGraph.append(ifStmVarString);
	dotGraph.append(thenStmLstString);
	dotGraph.append(elseStmLstString);
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << graphVector[graphVector.size()-1]) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 4) )->str()+'\n');
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter - 4) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 3) )->str()+'\n');//link if to var
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter - 4) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 2) )->str()+'\n');//link if to then stmlst
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter - 4) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 1) )->str()+'\n');//link if to else stmlst
	graphVector.push_back(counter - 1);
	graphVector.push_back(counter - 2);
}

//This method is used to setup the childs of an if else node
//PRE: line number, vector of twin, variable name, Node* if, Node* proc, Node*& then statementlist, Node*& else statementlist
//POST: none
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

//This method is used to create the while Node
//PRE: variable name, line number, vector of twin, string pointer for graph drawing, int, vector of int for graph drawing
//POST: none
void AST::createWhileNode(std::string varName, int lineNumber, std::vector<Twin*>& twinVector, std::string& dotGraph, int& counter, std::vector<int>& graphVector){

	Node* whileStm = nullptr;

	Node* procNode = twinVector.front()->getStmNode();

	Node* whileStmLst = nullptr;
	setupWhileVarListNode(lineNumber, twinVector, varName, whileStm, procNode, whileStmLst);

	Twin* tTwin = new Twin(whileStm, whileStmLst);

	twinVector[twinVector.size() - 1]->getStmListNode()->addStmt(whileStm);

	twinVector.push_back(tTwin);

	/*Graph Drawing*/
	std::string whileStmString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() +"[label=\":while\" shape=box];\n";
	std::string whileStmVarString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() + "[label=\""+ varName +":variable\" shape=box];\n";
	std::string whileStmGraphString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() + "[label=\":stmLst\" shape=box];\n";
	dotGraph.append(whileStmString);
	dotGraph.append(whileStmVarString);
	dotGraph.append(whileStmGraphString);
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << graphVector[graphVector.size()-1]) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 3) )->str()+'\n');
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter - 3) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 2) )->str()+'\n');
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter - 3) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 1) )->str()+'\n');
	graphVector.push_back(counter - 1);

}

//This method is used to setup the child of  the while Node
//PRE: line number, vector of twin, variable name, Node* while, Node* proc, Node*& while statementlist
//POST: none
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

//This method is used to extract the expression of a variable
//PRE: address of line, address vector of string
//POST: none
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

//This method is used to catch if there are multiple '=' in an expression
//PRE: line number, address of vector of string
//POST: none
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

//This method is used to create an assign node
//PRE: line, line number, vector of twin, string pointer for graph drawing, int, vector of int for graph drawing
//POST: none
void AST::createAssignNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector, unsigned i, std::string& dotGraph, int& counter, std::vector<int>& graphVector){
	std::vector<std::string> assignVector;// should have size of 2 only. Left and Right side without the equal
	extractExpressionVar(line, assignVector);
	catchExpressionException(i, assignVector);


	std::string varName = assignVector[0];
	expTree->catchInvalidNameException(varName);

	Node* procNode = twinVector.front()->getStmNode();
	Node* assignStm = nullptr;

	setupAssignVarListNode(assignVector[1], lineNumber, twinVector, i, varName, procNode, assignStm, dotGraph, counter, graphVector);

	twinVector[twinVector.size() - 1]->getStmListNode()->addStmt(assignStm);


}

//This method is used to create setup the childs of an assign node
//PRE: line, line number, vector of twin, string pointer for graph drawing, int, vector of int for graph drawing
//POST: none
void AST::setupAssignVarListNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector, unsigned i, std::string varName, Node* procNode, Node*& assignStm, std::string& dotGraph, int& counter, std::vector<int>& graphVector){

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

	Node* assignExp = expTree->exptreeSetup(pkb, postflix, lineNumber, assignStm, procNode, parentNode, dotGraph, counter, graphVector);

	assignStm->setRightChild(assignExp);


	/*Graph Drawing*/
	std::string assignStmString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() +"[label=\":assign\" shape=box];\n";
	std::string assignStmVarString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() + "[label=\""+ varName +":variable\" shape=box];\n";
	dotGraph.append(assignStmString);
	dotGraph.append(assignStmVarString);
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << graphVector[graphVector.size()-1]) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 2) )->str()+'\n');//link stmLst to assign
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter - 2) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 1) )->str()+'\n');//link assign to var
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter - 2) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 3) )->str()+'\n');// link assign to right var

}

//This method is used to pop the current parent out of a stack
//PRE: address stack of a string, address vecotr of twin
//POST: none
void AST::popImmediateParent(std::stack<std::string>& bracesStack, std::vector<Twin*>& twinVector){
	bracesStack.pop();
	twinVector.erase(twinVector.end() - 1);
}

//This method is used to create a call node
//PRE: statement name string, line number, current procedure name string, address vector or twin, line number, address string of graph, addres int, vector of int for graph drwaing
//POST: none
void AST::createCallNode(std::string stmName, int lineNumber, std::string& currentProcName, std::vector<Twin*>& twinVector, unsigned i, std::string& dotGraph, int& counter, std::vector<int>& graphVector){
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

	/*Graph Drawing*/
	std::string callStmString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() + "[label=\""+ stmName +":Call\" shape=box];\n";
	dotGraph.append(callStmString);
	dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << graphVector[graphVector.size()-1]) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << counter - 1) )->str()+'\n');
}

//This method is used to create a procedure node
//PRE: proc name string, vecotr of main program, address stack of strings, address vector of twin, line number, address string of graph, address int, address vector of int 
//POST: none
void AST::createProcNode(std::string& currentProcName, std::vector<Node*>& mainProg, std::stack<std::string>& bracesStack, std::vector<Twin*>& twinVector, unsigned i, std::string& dotGraph, int& counter, std::vector<int>& graphVector){	
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

	/*Graph Drawing*/
	std::string procStmString = static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() +"[label=\""+ currentProcName+":procedure\" shape=box];\n";
	std::string procStmGraphString = static_cast<ostringstream*>( &(ostringstream() << counter) )->str() + "[label=\":stmLst\" shape=box];\n";
	dotGraph.append(procStmString);
	dotGraph.append("prog->"+static_cast<ostringstream*>( &(ostringstream() << counter - 1) )->str()+'\n');
	dotGraph.append(procStmGraphString);
	std::string linkString = static_cast<ostringstream*>( &(ostringstream() << counter - 1) )->str() + "->" + static_cast<ostringstream*>( &(ostringstream() << counter) )->str() + '\n';
	dotGraph.append(linkString);
	graphVector.push_back(counter++);
}