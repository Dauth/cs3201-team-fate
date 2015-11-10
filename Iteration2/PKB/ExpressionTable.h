#ifndef EXPRESSIONTABLE_H
#define EXPRESSIONTABLE_H

#include "stdafx.h"
#include "Node.h"

class ExpressionTable {
	
	public:

		ExpressionTable();
		std::vector<Node*> getExpressions(std::string);
		std::vector<Node*> getRootExpressions(std::string);
		void addExpression(Node*);

	private:

		std::map<std::string, std::vector<Node*>> table;
};

#endif