#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H

#include "stdafx.h"
#include "PKB.h"

class ExpressionTree{
	PKB* pkb;

private:
	bool ExpressionTree::isOperand(std::string);
	bool ExpressionTree::isOperator(std::string);
	bool ExpressionTree::isAlpha(std::string);
	bool ExpressionTree::isDigit(std::string);
	int ExpressionTree::getPrecedence(std::string);
	synt_type ExpressionTree::getSyntType(std::string);

public:
	ExpressionTree(PKB*);
	bool ExpressionTree::isInflixBalanced(std::string);
	std::vector <std::string> ExpressionTree::expressionConverter(std::string);
	Node* ExpressionTree::exptreeSetup(std::vector<std::string>, int, Node*, Node*, Node*);
	Node* ExpressionTree::exptreeSetupSON(std::vector<std::string>);


	
};

#endif