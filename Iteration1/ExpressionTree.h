#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H

#include "stdafx.h"
#include "Node.h"
#include "PKB.h"

class ExpressionTree{
	PKB pkb;

private:
	static bool ExpressionTree::isOperand(char);
	static bool ExpressionTree::isOperator(char);
	static bool ExpressionTree::isAlpha(char);
	static bool ExpressionTree::isDigit(char);
	static int ExpressionTree::getPrecedence(char);
	bool ExpressionTree::isInflixBalanced(std::string);
	Node* ExpressionTree::insert(Node*, Node*, int, char, int, Node*, Node*, Node*);

public:
	static std::vector <char> ExpressionTree::expressionConverter(std::string);
	static Node* ExpressionTree::exptreeSetup(std::vector<char>, int, Node*, Node*, Node*);

};

#endif