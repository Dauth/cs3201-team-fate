#include "ExpressionTree.h"
#include <stack>
#include <string>
#include <vector>


ExpressionTree::ExpressionTree(){};

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
	if(cChar.compare("(") == 0){
		tStack.push(cChar);
	}else if (cChar.compare(")") == 0){
		while(!tStack.empty() && wChar = tStack.pop() != "("){
				resultVector.push_back(wChar);	//push all operators which reside within the brackets
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
int ExpressionTree::getPrecedence(char oOperator){
	int result = -1;
	if(oOperator.compare("*") == 0){
		result = 2;
	}else if(oOperator.compare("+") == 0){
		result = 1;
	}else if(oOperator.compare("-") == 0){
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
	stack <char> tStack;
	char cChar;

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


Node ExpressionTree::exptreeSetup(vector<string> postflixExp, int lineNo){
	Node operatorRoot = Node(expression, lineNo, postflixExp[postflixExp.size() - 1]);

	for(int i = postflixExp.size() - 2; i >= 0; i++){
		const char* expressionChar = postflixExp[i].c_str();
		if(isOperator(expressionChar) || expressionChar == "(" || expressionchar == ")"){
			Node tNode = Node(expression, lineNo, expressionChar);
			operatorRoot = insert(operatorRoot, tNode);
		}else if (isOperand(expressionChar)){
			Node tNode = Node(constant, lineNo, expressionChar);
			operatorRoot = insert(operatorRoot, tNode);
		}
	}
	return operatorRoot;
}

/*
This function builds the expression tree
Parameters: 
Return:		Node*
*/

Node* ExpressionTree::insert(Node root, Node node){
	if(root == NULL ){
		root = node;
	}
	else{
		if(root.getRightChild() == null){
			root.setRightChild(node);
			node.setParent(root);
			insert(root.getRightChild(), node);
		}else if(isOperator(root.getRightChild().getValue())){
			insert(root.getRightChild(), node);
		}else if(root.getLeftChild() == null){
			root.setLeftChild(node);
			node.setParent(root);
			insert(root.getLeftChild(), node);
		}else if (isOperator(root.getLeftChild().getValue())){
			insert(root.getLeftChild(), node);
		}
	}
	return root;
}




