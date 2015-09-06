#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H

#include "stdafx.h"
#include "PKB.h"

class ExpressionTree{
	PKB* pkb;

private:
	bool ExpressionTree::isOperand(char);
	bool ExpressionTree::isOperator(char);
	bool ExpressionTree::isAlpha(char);
	bool ExpressionTree::isDigit(char);
	int ExpressionTree::getPrecedence(char);
	bool ExpressionTree::isInflixBalanced(std::string);
	Node* ExpressionTree::insert(Node*, Node*, int, char, int, Node*, Node*, Node*);
	Node* ExpressionTree::insertSON(Node*, Node*);

public:
	ExpressionTree(PKB*);
	std::vector <char> ExpressionTree::expressionConverter(std::string);
	Node* ExpressionTree::exptreeSetup(std::vector<char>, int, Node*, Node*, Node*);
	Node* ExpressionTree::exptreeSetupSON(std::vector<char>);
	
};

#endif