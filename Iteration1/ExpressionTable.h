#ifndef EXPRESSIONTABLE_H
#define EXPRESSIONTABLE_H

#include "stdafx.h"
#include "Node.h"

class ExpressionTable {
	std::map<std::string, std::vector<Node*>> table;
	
public:
	ExpressionTable();
	std::vector<Node*> getExpressions(std::string);
	
	void addExpression(Node*);
};

#endif