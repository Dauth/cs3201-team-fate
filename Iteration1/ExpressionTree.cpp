#include "stdafx.h"
#include "ExpressionTree.h"
#include <stack>
#include <cstdlib>
#include <regex>
using std::regex;
using std::string;
using std::sregex_token_iterator;
#include <locale>

const int LEFT = 1;
const int RIGHT = 2;
const char* TIMES = "*";
const char* PLUS = "+";
const char* MINUS = "-";
const char* OPENBRACKETS = "(";
const char* CLOSEDBRACKETS = ")";
const char* OPENBRACES = "{";
const char* CLOSEDBRACES = "}";

ExpressionTree::ExpressionTree(PKB* p) {
	pkb = p;
}

void ExpressionTree::splitString(std::string inflixString, std::vector<std::string>& splittedString){
	std::string tempStr = "";
	for(int i = 0; i < inflixString.size(); i++){
		std::string expStr(1, inflixString[i]);
		if(isOperand(expStr)){
			tempStr.append(expStr);
		}else if(isOperator(expStr) || inflixString[i] == '(' || inflixString[i] == ')'){
			catchNameStartsLetterException(tempStr);
			splittedString.push_back(tempStr);
			tempStr.clear();
			tempStr.append(expStr);
			splittedString.push_back(tempStr);
			tempStr.clear();
		}
	}
	if(!tempStr.empty()){
		catchNameStartsLetterException(tempStr);
		splittedString.push_back(tempStr);
	}
}

/*
This function converts an inflix expression to postflix.
Parameters: String - a string that consists of operand and operator
Return:		array
*/
std::vector <std::string> ExpressionTree::expressionConverter(std::string inflixString){
	std::stack <std::string> tStack;
	std::vector <std::string> resultVector;
	std::vector<std::string> splittedString;
	std::string sString;
	
	splitString(inflixString, splittedString);

	for(unsigned int i = 0; i < splittedString.size(); i++){
		sString = splittedString[i];
		std::string wString;
		
		if(sString.compare("(") == 0){
			tStack.push(sString);
		}else if (sString.compare(")") == 0){
			while(!tStack.empty() && (!(tStack.top().compare("(") == 0))){
				wString = tStack.top();
				tStack.pop();
				resultVector.push_back(wString);	//push all operators which reside within the brackets					
			}
		tStack.pop();//to remove open brackets
		}

		if (isOperand(sString)){
			resultVector.push_back(sString);
		}
		
		if(isOperator(sString)){
			if(tStack.empty()){
				tStack.push(sString);
			}else{
				while(!tStack.empty() && (getPrecedence(sString) <= getPrecedence(tStack.top()))){
					resultVector.push_back(tStack.top());
					tStack.pop();
				}
				tStack.push(sString);
			}
		}
	}
  		while(!tStack.empty()){
			resultVector.push_back(tStack.top());
			tStack.pop();
		}
	return resultVector;
}









/*
This function checks if the given character is alphanumeric - A-z0-9.
Parameters: char
Return:		bool
*/
bool ExpressionTree::isOperand(std::string sString){
	bool result = true;
	for(int i = 0; i < sString.length(); i++){
		if(!isalnum(sString[i])){
			result = false;
			return result;
		}
	}
	return result;
}

bool ExpressionTree::isAlpha(std::string sString){
	bool result = true;

	for(int i = 0; i < sString.length(); i++){
		if(!isalpha(sString[i])){
			result = false;
			return result;
		}
	}
	return result;
}

bool ExpressionTree::isDigit(std::string sString){
	bool result = true;
	
	for(int i = 0; i < sString.length(); i++){
		if(!isdigit(sString[i])){
			result = false;
			return result;
		}
	}
	return result;
}

/*
This function checks if the given character is an operator  + - *.
Parameters: char
Return:		bool
*/
bool ExpressionTree::isOperator(std::string sString){
	bool result = false;

	for(int i = 0; i < sString.length(); i ++){
		const char* bChar = &sString[i];
		if(strcmp(bChar, TIMES) == 0 || strcmp(bChar, PLUS) == 0 || strcmp(bChar, MINUS) == 0){
		result = true;
		}
	}
	
	return result;
}



/*
This function gets the rank order of the operator.
Parameters: char
Return:		int
*/
int ExpressionTree::getPrecedence(std::string oOperator){
	int result = -1;
	const char *bChar = &oOperator[0];
	
	if(strcmp(bChar, TIMES) == 0){
		result = 2;
	}else if(strcmp(bChar, PLUS) == 0){
		result = 1;
	}else if(strcmp(bChar, MINUS) == 0){
		result = 1;
	}
	return result;
}

/*
This function checks if the given inflix expression is balanced a.k.a correct number of parenthesis
Parameters: string
Return:		bool
*/
bool ExpressionTree::isInflixBalanced(std::string inflixString){
	bool result = false;
	std::stack <char> tStack;
	char cChar;

	for(unsigned int i = 0; i < inflixString.length(); i++){
		cChar = inflixString[i];
		if(cChar == '('){
			tStack.push(cChar);
		}else if (cChar == ')'){
			if(!tStack.empty()){
				tStack.pop();
			}else{	//when more closing then opening brackets
				return result;
			}
		}
	}
	//same number of opening and closing brackets
	if(tStack.empty()){
		result = true;
	}

	return result;//if it is false here, there are more opening then closing brackets
}

synt_type ExpressionTree::getSyntType(std::string expressionStr){
	synt_type expressionCharType;
	if(isAlpha(expressionStr)){
			expressionCharType = variable;
		}else if(isDigit(expressionStr)){
			expressionCharType = constant;
		} else {
			expressionCharType = expression;
		}
		return expressionCharType;
}

Node* ExpressionTree::exptreeSetup(std::vector<std::string> postflixExp, int lineNo, Node* assignStmNode, Node* procNode, Node* parentNode){
	synt_type expressionCharType;
	std::stack<Node*> operandStack;

	for(int i = 0; i < postflixExp.size(); i++){
		std::string expressionChar = postflixExp[i];
		expressionCharType = getSyntType(postflixExp[i]);

		if(isOperand(postflixExp[i])){
			Node* tNode = pkb->createNode(expressionCharType, lineNo, expressionChar, assignStmNode, nullptr, parentNode, procNode);
			operandStack.push(tNode);
		}else if (isOperator(postflixExp[i])){
			Node* tNode = pkb->createNode(expressionCharType, lineNo, expressionChar, assignStmNode, nullptr, parentNode, procNode);
			tNode->setRightChild(operandStack.top());
			operandStack.pop();

			tNode->setLeftChild(operandStack.top());
			operandStack.pop();

			operandStack.push(tNode);
		}
	}

	return operandStack.top();
}

Node* ExpressionTree::exptreeSetupSON(std::vector<std::string> postflixExp){
	synt_type expressionCharType;
	std::stack<Node*> operandStack;

	for(int i = 0; i < postflixExp.size(); i++){
		std::string expressionStr= postflixExp[i];
		expressionCharType = getSyntType(postflixExp[i]);
		Node* tNode = new Node(expressionCharType, 0, expressionStr);

		if(isOperand(postflixExp[i])){
			operandStack.push(tNode);
		}else if (isOperator(postflixExp[i])){
			operandStack.top()->setParent(tNode);
			tNode->setRightChild(operandStack.top());
			operandStack.pop();

			operandStack.top()->setParent(tNode);
			tNode->setLeftChild(operandStack.top());
			operandStack.pop();

			operandStack.push(tNode);
		}
	}

	return operandStack.top();
}

bool ExpressionTree::isNameStartWithLetter(std::string input){
	std::regex assignStm("\\s*\\t*^[A-Za-z]");
	std::smatch match;

	int result = false;

	if (std::regex_search(input, match, assignStm) || isDigit(input)){
		result = true;
	}
	return result;
}

void ExpressionTree::catchNameStartsLetterException(std::string line){
	try{
		if(!isNameStartWithLetter(line)){
			throw line;
		}
	}catch(std::string e){
		std::cout<<"NAME DOES NOT START WITH A LETTER: "<<e<<std::endl;
		std::terminate();
	}
}