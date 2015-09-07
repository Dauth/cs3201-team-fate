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

std::vector<Node*> ExpressionTable::getRootExpressions(std::string exprstr, Variable* var) {
	std::vector<Node*> exprs = table[exprstr];
	std::vector<Node*> exprsfiltered;
	for (int i=0; i<exprs.size(); i++) {
		if(exprs[i]->getVariable() == var && exprs[i]->getParent()->getType() == assignment) {
			exprsfiltered.push_back(exprs[i]);
		}
	}
	return exprsfiltered;
}

void ExpressionTable::addExpression(Node* node) {
	if ( table.find(node->getValue()) == table.end() ) {
		std::vector<Node*> nodes;
		table[node->getValue()] = nodes;
	}
	table[node->getValue()].push_back(node);
}

void debug() {
	//std::cout << "ExpressionTable: " << table.size() << "\n";
}