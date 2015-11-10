#include "stdafx.h"
#include "ExpressionTable.h"

// Expression table keeps track of expression nodes in assignment statements
// this is used to speed up pattern matching
ExpressionTable::ExpressionTable () {

}

// get all expression nodes of a provided expression
std::vector<Node*> ExpressionTable::getExpressions(std::string exprstr) {
	return table[exprstr];
}

// gets expression nodes which are the direct child of an assignment statement
std::vector<Node*> ExpressionTable::getRootExpressions(std::string exprstr) {
	std::vector<Node*> exprs = table[exprstr];
	std::vector<Node*> exprsfiltered;
	for (int i=0; i<exprs.size(); i++) {
		if(exprs[i]->getParent()->getType() == assignment) {
			exprsfiltered.push_back(exprs[i]);
		}
	}
	return exprsfiltered;
}

// adds an expression
void ExpressionTable::addExpression(Node* node) {
	if ( table.find(node->getValue()) == table.end() ) {
		std::vector<Node*> nodes;
		table[node->getValue()] = nodes;
	}
	table[node->getValue()].push_back(node);
}