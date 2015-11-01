#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H

#include "stdafx.h"
#include "PKB.h"

class PKB;

class ExpressionTree{

private:
	bool ExpressionTree::isOperand(std::string);
	bool ExpressionTree::isOperator(std::string);
	bool ExpressionTree::isAlpha(std::string);
	bool ExpressionTree::isDigit(std::string);
	int ExpressionTree::getPrecedence(std::string);
	SyntType ExpressionTree::getSyntType(std::string);

public:
	ExpressionTree();
	void catchUnknownOperatorException(int i);
	void splitString(std::string inflixString, std::vector<std::string>& splittedString);
	bool ExpressionTree::isInflixBalanced(std::string);
	std::vector <std::string> ExpressionTree::expressionConverter(std::string);
	Node* ExpressionTree::exptreeSetup(PKB*, std::vector<std::string>, int, Node*, Node*, Node*, std::string& dotGraph, int& counter, std::vector<int>& graphVector);
	Node* ExpressionTree::exptreeSetupSON(std::vector<std::string>);
	bool ExpressionTree::isNameValid(std::string);
	void ExpressionTree::catchInvalidNameException(std::string line);

	
};

#endif