#include <algorithm>
#include <vector>
#include <string>
#include <stack>
#include "PKB.h"
#include "synt_type.h"
#include "node.h"
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

AST::AST(){
	PKB pkb;
}

/*
This function processes a vector which contains the source file
Parameters: vector<string>
Return:		??
*/
std::vector<Node*> AST::buildAST(vector<string> sourceVector){
	std::string line;
	std::string currentProcName;
	std:vector<Node*> mainProg;
	std::stack<string> bracesStack;
	std::vector<Node*> familyVector;
	int bracesNo = -1;
	
	for(int i = 0; i < sourceVector.size(); i++){
			line = sourceVector[i];
			int statementType = getStatementType(line);
			if(statementType == PROCEDURESTM && bracesStack.empty()){
				currentProcName = extractStatementPart(PROCEDURESTM, line);
				Node* procStm = pkb.createProcedure(currentProcName);
				mainProg.push_back(procStm);

				bracesStack.push("{");
				familyVector.push_back(procStm);

			}else if(statementType == PROCEDURESTM && !bracesStack.empty()){
				cout<<"----PROCEDURE WITHIN A PROCEDURE----";
			}else{
				if(statementType == WHILESTM){
					string varName = extractStatementPart(WHILESTM, line);
					Node* whileStm = Node(whileLoop, i + 1, ":while");
					Node* tNodeLeft = Node(variable, i + 1, varName);
					Node* tNodeRight = Node(statementList, i + 1, "stmLst");

					whileStm.setLeftChild(tNodeLeft);
					whileStm.setRightChild(tNodeRight);

					familyVector.push_back(whileStm);

				}else if(getStatementType(line) == CALLSTM){
					std::string callProcName = extractStatementPart(CALLSTM, line);

					if(callProcName.compare(currentProcName) == 0){
						cout<<"----RECURSIVE CALL NOT ALLOWED----";
					}

					Node* tCall = Node(call, i + 1, callProcName + ":call");

					tCall.setParent(familyVector[familyVector.size() - 1]);
					tCall.setRoot(familyVector.front());

				}else if(statementType == ASSIGNSTM){
						Node* assignStm = Node(assignment, i + 1, ":assign");
						Node* leftNode = Node(variable, i + 1, extractStatementPart(ASSIGNSTMVAR, line));

						assignStm.setLeftChild(leftNode);
						leftNode.setParent(familyVector[familyVector.size() - 1]);
						leftNode.setRoot(familyVector.front());//No proc within a proc, therefore base is always a proc
						
						std::string inflix = extractStatementPart(ASSIGNSTMEXP, line);
						std::vector<string> postflix = expressionConverter(inflix);
						
						Node* rightNode = exptreeSetup(postflix, i + 1);
						assignStm.setRightChild(rightNode);
						rightNode.setParent(familyVector[familyVector.size() - 1]);
						rightNode.setRoot(familyVector.front());
						
				}
				
			}

			if(bracesNo = getNumOfClosingbraces()){
				if(bracesNo > bracesStack.size()){
					cout<<"ERROR IN SOURCE CODE, TOO MANY CLOSING BRACES";
				}
				while(bracesNo > 0){
					bracesStack.pop();
					familyVector.erase(familyVector.end() - 1);
				}
			}
	}
	return mainProg;
}

int AST::getStatementType(string input){
	std::regex procedure("procedure\s+\w+\s*{$");
	std::regex callProc("call\s+(\w+);}*$");
	std::regex whileLoop("while\s+\w*\s*{$");
	std::regex assignStm("\w+=[A-z0-9*+-\s]+;}*$");

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

std::string AST::extractStatementPart(int inputType, string input){
	std::regex procedureName("procedure\s+(\w+)\s*{$");
	std::regex callProcName("call\s+(\w+);}*$");
	std::regex whileLoopVar("while\s+(\w*)\s*{$");
	std::regex assignStmLeftHand("(\w+)=[A-z0-9*+-]+;}*$");
	std::regex assignStmRightHand("(\w+=([A-z0-9*+-\s]+);}*$");
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

int AST::getNumOfClosingbraces(string input){
	std::regex closingBraces(";}*$");
	std::smatch match;

	int result = -1;

	if(std::regex_search(input, match, closingBraces)){
		result = match[0].length();
	}

	return result;
}



