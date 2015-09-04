#include "ExpressionTable.h"
#include "stdafx.h"

ExpressionTable::ExpressionTable () {

}

std::vector<Node*> ExpressionTable::getExpressions(std::string exprstr) {
	return table[exprstr];
}

void ExpressionTable::addExpression(Node* node) {
	if ( table.find(node->getValue()) == table.end() ) {
		std::vector<Node*> pnodes;
		table[node->getValue()] = pnodes;
	} else {
		table[node->getValue()].push_back(node);
	}
}