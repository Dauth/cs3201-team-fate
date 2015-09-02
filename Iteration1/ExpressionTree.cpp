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
	char wChar;
	for(int i = 0; i < inflixString.length(); i++){
		cChar = inflixString[i];
		if(cChar == "("){
			tStack.push(cChar);
		}else if (cChar == ")"){
			while(wChar = tStack.pop() != "(" && !tStack.empty()){
				resultVector.push_back(wChar)	//push all operators which reside within the brackets
			}
		}
		if(isOperator(cChar)){
			if(tStack.empty()){
				tStack.push(cChar);
			}else if(!tStack.empty()){
				if(getPrecedence(cChar) > getPrecedence(tStack.top())){
					tStack.push(cChar);
				}else if(getPrecedence(cChar) <= getPrecedence(tStack.top())){
					while(!tStack.empty()){
						resultVector.push_back(tStack.pop());
					}
					tStack.push(cChar);
				}
			}
		}else if (isOperand(cChar)){
			resultVector.push_back(cChar);
		}
	}
	return resultVector;
}

/*
This function checks if the given character is alphanumeric - A-z0-9.
Parameters: char
Return:		bool
*/
bool isOperand(char cChar){
	bool result = false;
	if(isalnum(cChar)){
		result = true;
	}
	return result;
}

/*
This function checks if the given character is an operator  + - *.
Parameters: char
Return:		bool
*/
bool isOperator(char cChar){
	bool result = false;
	if(cChar == '(' || cChar == '*' || cChar == '+' || cChar == '-' ||){
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


#endif