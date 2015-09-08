#include "stdafx.h"
#include "ExpressionTree.h"
#include <stack>
#include <cstdlib>

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

/*
This function converts an inflix expression to postflix.
Parameters: String - a string that consists of operand and operator
Return:		array
*/
std::vector <char> ExpressionTree::expressionConverter(std::string inflixString){
	std::stack <char> tStack;
	std::vector <char> resultVector;
	char cChar;
	
	for(unsigned int i = 0; i < inflixString.length(); i++){
		cChar = inflixString[i];
		const char *bChar = &cChar;
		char wChar;
		
		if(cChar == '('){
			tStack.push(cChar);
		}else if (cChar == ')'){
			while(!tStack.empty() && tStack.top() != '('){
				wChar = tStack.top();
				tStack.pop();
				const char *dChar = &wChar;
				resultVector.push_back(wChar);	//push all operators which reside within the brackets					
			}
		tStack.pop();//to remove open brackets
		}

		if (isOperand(cChar)){
			resultVector.push_back(cChar);
		}
		
		if(isOperator(cChar)){
			if(tStack.empty()){
				tStack.push(cChar);
			}else{
				while(!tStack.empty() && (getPrecedence(cChar) <= getPrecedence(tStack.top()))){
					resultVector.push_back(tStack.top());
					tStack.pop();
				}
				tStack.push(cChar);
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
bool ExpressionTree::isOperand(char cChar){
	bool result = false;
	if(isalnum(cChar) != 0){
		result = true;
	}
	return result;
}

bool ExpressionTree::isAlpha(char cChar){
	bool result = false;
	if(isalpha(cChar) != 0){
		result = true;
	}
	return result;
}

bool ExpressionTree::isDigit(char cChar){
	bool result = false;
	if(isdigit(cChar) != 0){
		result = true;
	}
	return result;
}

/*
This function checks if the given character is an operator  + - *.
Parameters: char
Return:		bool
*/
bool ExpressionTree::isOperator(char cChar){
	bool result = false;
	const char *bChar = &cChar;
	
	if(strcmp(bChar, TIMES) == 0 || strcmp(bChar, PLUS) == 0 || strcmp(bChar, MINUS) == 0){
		result = true;
	}
	return result;
}



/*
This function gets the rank order of the operator.
Parameters: char
Return:		int
*/
int ExpressionTree::getPrecedence(char oOperator){
	int result = -1;
	const char *bChar = &oOperator;
	
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

synt_type ExpressionTree::getSyntType(char expressionChar){
	synt_type expressionCharType;
	if(isAlpha(expressionChar)){
			expressionCharType = variable;
		}else if(isDigit(expressionChar)){
			expressionCharType = constant;
		} else {
			expressionCharType = expression;
		}
		return expressionCharType;
}

Node* ExpressionTree::exptreeSetup(std::vector<char> postflixExp, int lineNo, Node* assignStmNode, Node* procNode, Node* parentNode){
	synt_type expressionCharType;
	std::stack<Node*> operandStack;

	for(int i = 0; i < postflixExp.size(); i++){
		std::string expressionChar(1, postflixExp[i]);
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

Node* ExpressionTree::exptreeSetupSON(std::vector<char> postflixExp){
	synt_type expressionCharType;
	std::stack<Node*> operandStack;

	for(int i = 0; i < postflixExp.size(); i++){
		std::string expressionChar(1, postflixExp[i]);
		expressionCharType = getSyntType(postflixExp[i]);
		Node* tNode = new Node(expressionCharType, 0, expressionChar);

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


