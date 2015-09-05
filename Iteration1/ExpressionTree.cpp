#include "ExpressionTree.h"
#include <stack>
#include <string>
#include <vector>
#include "PKB.h"
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

ExpressionTree::ExpressionTree(){PKB pkb;};

using namespace std;

/*
This function converts an inflix expression to postflix.
Parameters: String - a string that consists of operand and operator
Return:		array
*/
std::vector <char> ExpressionTree::expressionConverter(std::string inflixString){
	stack <char> tStack;
	std::vector <char> resultVector;
	char cChar;
	
	for(int i = 0; i < inflixString.length(); i++){
		cChar = inflixString[i];
		const char *bChar = new char(cChar);
		char wChar;
		
		if(strcmp(bChar, OPENBRACKETS) == 0){
			tStack.push(cChar);
		}else if (strcmp(bChar, CLOSEDBRACKETS) == 0){
			while(!tStack.empty()){
				 wChar = tStack.top();
				 tStack.pop();
				 const char *dChar = new char(wChar);
				 if(strcmp(dChar, OPENBRACKETS) == 0){
					resultVector.push_back(wChar);	//push all operators which reside within the brackets
				 }else{
					 tStack.pop();
				 }
			}
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

		while(!tStack.empty()){
			resultVector.push_back(tStack.top());
			tStack.pop();
		}
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
	return isalpha(cChar);
}

bool ExpressionTree::isDigit(char cChar){
	return isdigit(cChar);
}

/*
This function checks if the given character is an operator  + - *.
Parameters: char
Return:		bool
*/
bool ExpressionTree::isOperator(char cChar){
	bool result = false;
	const char *bChar = new char(cChar);
	
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
	const char *bChar = new char(oOperator);
	
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
	stack <std::string> tStack;
	std::string cChar;

	for(int i = 0; i < inflixString.length(); i++){
		cChar = inflixString[i];
		if(cChar.compare("(") == 0){
			tStack.push(cChar);
		}else if (cChar.compare(")") == 0){
			if(!tStack.empty()){
				tStack.pop();
			}else{	//e.g  5+)6+7
				return result;
			}
		}
	}

	if(tStack.empty()){
		result = true;
	}

	return result;
}


Node* ExpressionTree::exptreeSetup(std::vector<char> postflixExp, int lineNo, Node* assignStmNode, Node* procNode, Node* parentNode){
	std::string str(1, postflixExp[postflixExp.size() - 1]);
	Node* operatorRoot = pkb.createNode(expression, lineNo, str);
	
	for(int i = postflixExp.size() - 2; i >= 0; i++){
		char expressionChar = postflixExp[i];

		if(isOperand(expressionChar) || isOperator(expressionChar)){//ignore brackets and other stuff
			operatorRoot = insert(operatorRoot, operatorRoot, -1, expressionChar, lineNo, assignStmNode, procNode, parentNode);
		}
	}
	return operatorRoot;
}

/*
This function builds the expression tree
Parameters: 
Return:		Node*
*/

Node* ExpressionTree::insert(Node* root, Node* dupRoot, int leftOrRight, char expressionChar, int lineNo, Node* assignStmNode, Node* procNode, Node* parentNode){
	synt_type expressionCharType;
	if(root == nullptr){
		if(isAlpha(expressionChar)){
			expressionCharType = variable;
		}else if(isDigit(expressionChar)){
			expressionCharType = constant;
		}

		root->setRoot(procNode);
		
		std::string str(1, expressionChar);
		if(leftOrRight == LEFT){
			root = pkb.createNode(expressionCharType, lineNo, str, assignStmNode, nullptr, parentNode, procNode);
			dupRoot->setLeftChild(root);

		}else if (leftOrRight == RIGHT){
			root = pkb.createNode(expressionCharType, lineNo, str, assignStmNode, nullptr, parentNode, procNode);
			dupRoot->setRightChild(root);
		}
	}
	else{
		if(root->getRightChild() == NULL || isOperator(root->getRightChild()->getValue()[0])){
			root = insert(root->getRightChild(), root, RIGHT, expressionChar, lineNo, assignStmNode, procNode, parentNode);
		}else if(root->getLeftChild() == NULL && isOperator(root->getLeftChild()->getValue()[0])){
			root = insert(root->getLeftChild(), root, LEFT, expressionChar, lineNo, assignStmNode, procNode, parentNode);
		}
	}
	return root;
}


Node* ExpressionTree::exptreeSetupSON(std::vector<char> postflixExp){
	std::string str(1, postflixExp[postflixExp.size() - 1]);
	Node* operatorRoot = new Node(expression, str);

	for(int i = postflixExp.size() - 2; i >= 0; i++){
		std::string expressionChar(1, postflixExp[i]);
		if(isOperator(postflixExp[i])){
			Node* tNode = new Node(expression, expressionChar);
			operatorRoot = insertSON(operatorRoot, tNode);
		}else if (isOperand(postflixExp[i])){
			Node* tNode = new Node(constant, expressionChar);
			operatorRoot = insertSON(operatorRoot, tNode);
		}
	}
	return operatorRoot;
}

/*
This function builds the expression tree
Parameters: 
Return:		Node*
*/

Node* ExpressionTree::insertSON(Node* root, Node* node){
	if(root == nullptr ){
		root = node;
	}
	else{
		if(root->getRightChild() == nullptr){
			root->setRightChild(node);
			node->setParent(root);
			insertSON(root->getRightChild(), node);
		}else if(isOperator(root->getRightChild()->getValue()[0])){
			insertSON(root->getRightChild(), node);
		}else if(root->getLeftChild() == nullptr){
			root->setLeftChild(node);
			node->setParent(root);
			insertSON(root->getLeftChild(), node);
		}else if (isOperator(root->getLeftChild()->getValue()[0])){
			insertSON(root->getLeftChild(), node);
		}
	}
	return root;
}

