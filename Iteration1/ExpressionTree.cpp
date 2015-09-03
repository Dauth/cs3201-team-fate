#include "ExpressionTree.h"
#include <stack>
#include <string>
#include <vector>

using namespace std;



/*
This function converts an inflix expression to postflix.
Parameters: String - a string that consists of operand and operator
Return:		array
*/
std::vector <char> expressionConverter(string inflixString){
	stack <char> tStack;
	std::vector <char> resultVector;
	char cChar;

	for(int i = 0; i < inflixString.length(); i++){
		cChar = inflixString[i];

		inflixBracketProcess(tStack, resultVector, cChar);
		inflixOperandProcess(tStack, resultVector, cChar);
		inflixOperatorProcess(tStack, resultVector, cChar);
	}

	while(!tStack.empty()){
		resultVector.push_back(tStack.pop());
	}

	return resultVector;
}

void inflixBracketProcess(stack <char>& tStack, std::vector <char>& resultVector, char cChar){
	char wChar;
	if(cChar == "("){
		tStack.push(cChar);
	}else if (cChar == ")"){
		while(!tStack.empty() && wChar = tStack.pop() != "("){
				resultVector.push_back(wChar)	//push all operators which reside within the brackets
		}
	}
}

void inflixOperandProcess(stack <char>& tStack, std::vector <char>& resultVector, char cChar){
	if (isOperand(cChar)){
			resultVector.push_back(cChar);
	}
}

void inflixOperatorProcess(stack <char>& tStack, std::vector <char>& resultVector, char cChar){
	if(isOperator(cChar){
		if(tStack.empty()){
			tStack.push(cChar);
		}else{
			while(!tStack.empty() && 
				(getPrecedence(cChar) <= getPrecedence(tStack.top()))){
					resultVector.push_back(tStack.pop());
			}
			tStack.push(cChar);
		}
	}
}


/*
This function checks if the given character is alphanumeric - A-z0-9.
Parameters: char
Return:		bool
*/
bool isOperand(char cChar){
	return isalnum(cChar);
}

/*
This function checks if the given character is an operator  + - *.
Parameters: char
Return:		bool
*/
bool isOperator(char cChar){
	bool result = false;
	if(cChar == '*' || cChar == '+' || cChar == '-' ||){
		result = true;
	}
	return result;
}

/*
This function gets the rank order of the operator.
Parameters: char
Return:		int
*/
int getPrecedence(char operator){
	int result = -1;
	if(operator == "*"){
		result = 2;
	}else if(operator == "+"){
		result = 1;
	}else if(operator == "-"){
		result = 1;
	}
	return result;
}

/*
This function checks if the given inflix expression is balanced a.k.a correct number of parenthesis
Parameters: string
Return:		bool
*/
bool isInflixBalanced(string inflixString){
	bool result = false;
	stack <char> tStack;
	char cChar;

	for(int i = 0; i < inflixString.length(); i++){
		cChar = inflixString[i];
		if(cChar == "("){
			tStack.push(cChar);
		}else if(cChar == ")"){
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

	return result
}

/*
This function checks if the given inflix expression well defined
Parameters: string
Return:		bool
*/
bool isInflixBalanced(string inflixString){
	bool result = false;
	stack <char> tStack;
	char cChar;
}

#endif