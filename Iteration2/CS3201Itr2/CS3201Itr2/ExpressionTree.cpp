#include "stdafx.h"
#include "ExpressionTree.h"
#include <stack>
#include <cstdlib>
#include <regex>
using std::regex;
using std::string;
using std::sregex_token_iterator;
#include <locale>
#include "PKB.h"

const int LEFT = 1;
const int RIGHT = 2;
const char* TIMES = "*";
const char* PLUS = "+";
const char* MINUS = "-";
const char* OPENBRACKETS = "(";
const char* CLOSEDBRACKETS = ")";
const char* OPENBRACES = "{";
const char* CLOSEDBRACES = "}";

ExpressionTree::ExpressionTree() {

}

//This method is used to catch an unknown operator in expression
//PRE: line number
//POST: none
void ExpressionTree::catchUnknownOperatorException(int i){
	std::cout<<"UNKNOWN OPERATOR DETECTED IN ASSIGNMENT AT LINE NO:"<<i + 1<<std::endl;
	std::terminate();
}

//This method is used to split a string into chracters
//PRE: string of infix, address vector of string
//POST: none
void ExpressionTree::splitString(std::string infixString, std::vector<std::string>& splittedString){
	std::string tempStr = "";
	for(int i = 0; i < infixString.size(); i++){
		std::string expStr(1, infixString[i]);

		if(isOperand(expStr)){
			tempStr.append(expStr);
		}else if(isOperator(expStr) || infixString[i] == '(' || infixString[i] == ')'){
			if(!tempStr.empty()){
				catchInvalidNameException(tempStr);
				splittedString.push_back(tempStr);
				tempStr.clear();
			}
			tempStr.append(expStr);
			splittedString.push_back(tempStr);
			tempStr.clear();
		}else{
			catchUnknownOperatorException(i);
		}
	}
	if(!tempStr.empty()){
		catchInvalidNameException(tempStr);
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
	return isalpha(sString[0]);
}

bool ExpressionTree::isDigit(std::string sString){
	return isdigit(sString[0]);
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

//This method is used to get the syntype of a character
//PRE: string of expression
//POST: none
SyntType ExpressionTree::getSyntType(std::string expressionStr){
	SyntType expressionCharType;
	if(isAlpha(expressionStr)){
			expressionCharType = variable;
		}else if(isDigit(expressionStr)){
			expressionCharType = constant;
		} else {
			expressionCharType = expression;
		}
		return expressionCharType;
}

//This method is used to create an assign node
//PRE: line, line number, vector of twin, string pointer for graph drawing, int, vector of int for graph drawing
//POST: none
Node* ExpressionTree::exptreeSetup(PKB* pkb, std::vector<std::string> postflixExp, int lineNo, Node* assignStmNode, Node* procNode, Node* parentNode, std::string& dotGraph, int& counter, std::vector<int>& graphVector){
	SyntType expressionCharType;
	std::stack<Node*> operandStack;
	std::stack<int> graphStack;//for graph drawing purposes

	for(int i = 0; i < postflixExp.size(); i++){
		std::string expressionChar = postflixExp[i];
		expressionCharType = getSyntType(postflixExp[i]);

		if(isOperand(postflixExp[i])){
			Node* tNode = pkb->createNode(expressionCharType, lineNo, expressionChar, assignStmNode, nullptr, parentNode, procNode);
			operandStack.push(tNode);
			/*Graph drawing*/
			dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() + "[label=\""+ expressionChar +"\" shape=box];\n");
			graphStack.push(counter - 1);
			//////////////////////////
		}else if (isOperator(postflixExp[i])){
			Node* tNode = pkb->createNode(expressionCharType, lineNo, expressionChar, assignStmNode, nullptr, parentNode, procNode);
			tNode->setRightChild(operandStack.top());
			operandStack.pop();

			tNode->setLeftChild(operandStack.top());
			operandStack.pop();

			operandStack.push(tNode);

			/*Graph drawing*/
			dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter++) )->str() + "[label=\""+ expressionChar +"\" shape=box];\n");
			int rightChild = graphStack.top();
			graphStack.pop();
			int leftChild = graphStack.top();
			graphStack.pop();
			dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter - 1) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << leftChild) )->str()+'\n');
			dotGraph.append(static_cast<ostringstream*>( &(ostringstream() << counter - 1) )->str()+"->"+static_cast<ostringstream*>( &(ostringstream() << rightChild) )->str()+'\n');
			graphStack.push(counter - 1);
			//////////////////////////
		}
	}

	return operandStack.top();
}

//This method is used to setup a expression tree for pattern matching
//PRE: vecotr of string for postfix 
//POST: none
Node* ExpressionTree::exptreeSetupSON(std::vector<std::string> postfixExp){
	SyntType expressionCharType;
	std::stack<Node*> operandStack;

	for(int i = 0; i < postfixExp.size(); i++){
		std::string expressionStr= postfixExp[i];
		expressionCharType = getSyntType(postfixExp[i]);
		Node* tNode = new Node(expressionCharType, "0", expressionStr);

		if(isOperand(postfixExp[i])){
			operandStack.push(tNode);
		}else if (isOperator(postfixExp[i])){
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

//This method is used to check first letter is a alphabet and subsequet letters are alphanumeric
//PRE: line, line number, vector of twin, string pointer for graph drawing, int, vector of int for graph drawing
//POST: none
bool ExpressionTree::isNameValid(std::string input){
	bool result = false;
	bool isAlphaPresent = false;
	bool isDigitPresent = false;

	for(int i = 0; i < input.length(); i++){
		char cChar = input[i];
		if(isalpha(cChar)){
			isAlphaPresent = true;
		}else if(isdigit(cChar)){
			isDigitPresent = true;
		}else{
			result = false;
			return result;
		}
	}

	if(isDigitPresent && isAlphaPresent){	//a56pple123  not  344apple
		if(isalpha(input[0])){
			result = true;
		}
	}else if(!isDigitPresent && isAlphaPresent){	//apple
			result = true;
	}else if(isDigitPresent && !isAlphaPresent){	//123
			result = true;
	}
	return result;
}

//This method is used to check if the name is invalid
//PRE: line
//POST: none
void ExpressionTree::catchInvalidNameException(std::string line){
	try{
		if(!isNameValid(line)){
			throw line;
		}
	}catch(std::string e){
		std::cout<<"FIRST LETTER MUST BE ALPHABELT ONLY AND SUBSEQUENT CHARACTERS CAN BE ALPHANUMERIC: "<<e<<std::endl;
		std::terminate();
	}
}