#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H

#include "stdafx.h"
#include "Node.h"



class ExpressionTree{
public:

	static std::vector <string> ExpressionTree::expressionConverter(std::string);
	static Node* ExpressionTree::exptreeSetup(std::vector<string> postflixExp, int);

};

#endif