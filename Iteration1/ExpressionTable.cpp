#include "stdafx.h"
#include "ExpressionTable.h"

ExpressionTable::ExpressionTable () {

}

std::vector<Node*> ExpressionTable::getExpressions(std::string exprstr) {
	return table[exprstr];
}

std::vector<Node*> ExpressionTable::getExpressions(std::string exprstr, Variable* var) {
	std::vector<Node*> exprs = table[exprstr];
	std::vector<Node*> exprsfiltered;
	for (int i=0; i<exprs.size(); i++) {
		if(exprs[i]->getVariable() == var) {
			exprsfiltered.push_back(exprs[i]);
		}
	}
	return exprsfiltered;
}

void ExpressionTable::addExpression(Node* node) {
	if ( table.find(node->getValue()) == table.end() ) {
		std::vector<Node*> pnodes;
		table[node->getValue()] = pnodes;
	} else {
		table[node->getValue()].push_back(node);
	}
}

void debug() {
	//std::cout << "ExpressionTable: " << table.size() << "\n";
}