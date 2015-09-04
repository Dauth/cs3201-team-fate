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
std::vector <char> ExpressionTree::expressionConverter(string inflixString){
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

void ExpressionTree::inflixBracketProcess(stack <char>& tStack, std::vector <char>& resultVector, char cChar){
	char wChar;
	if(cChar == "("){
		tStack.push(cChar);
	}else if (cChar == ")"){
		while(!tStack.empty() && wChar = tStack.pop() != "("){
				resultVector.push_back(wChar)	//push all operators which reside within the brackets
		}
	}
}

void ExpressionTree::inflixOperandProcess(stack <char>& tStack, std::vector <char>& resultVector, char cChar){
	if (isOperand(cChar)){
			resultVector.push_back(cChar);
	}
}

void ExpressionTree::inflixOperatorProcess(stack <char>& tStack, std::vector <char>& resultVector, char cChar){
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
bool ExpressionTree::isOperand(char cChar){
	return isalnum(cChar);
}

/*
This function checks if the given character is an operator  + - *.
Parameters: char
Return:		bool
*/
bool ExpressionTree::isOperator(char cChar){
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
int ExpressionTree::getPrecedence(char operator){
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
bool ExpressionTree::isInflixBalanced(string inflixString){
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


Node* ExpressionTree::exptreeSetup(vector<string> postflixExp, int lineNo){
	Node* operatorRoot = Node(expression, lineNo, postflixExp[postflixExp.length() - 1]);

	for(int i = postflixExp.length() - 2; i >= 0; i++){
		char expressionChar = postflixExp[i];
		Node* tNode;
		if(isOperator(expressionChar) || expressionChar == "(" || expressionchar == ")"){
			tNode = Node(expression, lineNo, expressionChar);
		}else if (isOperand(expressionChar)){
			tNode = Node(constant, lineNo, expressionChar);
		}

		operatorRoot = insert(operatorRoot, tNode);
	}
	return operatorRoot;
}

/*
This function builds the expression tree
Parameters: 
Return:		Node*
*/

Node* ExpressionTree::insert(root, node){
	if(root == null){
		root = node
	}
	else{
		if(root.getRightChild() == null){
			root.setRightChild() = node
			node.setParent() = root
			insert(root.getRightChild(), node)
		}else if(isOperator(root.getRightChild().getValue())){
			insert(root.getRightChild(), node)
		}else if(root.getLeftChild() == null){
			root.setLeftChild() = node
			node.setParent() = root
			insert(root.getLeftChild(), node)
		}else if (isOperator(root.getLeftChild().getValue())){
			insert(root.getLeftChild(), node)
		}
	}
	return root;
}




#endif	