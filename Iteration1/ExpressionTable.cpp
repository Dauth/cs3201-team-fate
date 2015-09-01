#include "ExpressionTable.h"

ExpressionTable::ExpressionTable () {

}

std::vector<Node*> ExpressionTable::getExpressions(std::string exprstr) {
	return table[exprstr];
}

void ExpressionTable::addExpression(Node* pnode) {
	Node node = *pnode;
	if ( table.find(node.getId()) == table.end() ) {
		std::vector<Node*> pnodes;
		table[node.getId()] = pnodes;
	} else {
		table[node.getId()].push_back(pnode);
	}
}