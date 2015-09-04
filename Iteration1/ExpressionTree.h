#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H

#include "stdafx.h"
#include "Node.h"



class ExpressionTree{





public:
	static std::vector <String> ExpressionTree::expressionConverter(std::string inflixString);
	static Node* ExpressionTree::exptreeSetup(std::vector<String> postflixExp, int lineNo);






};

#endif