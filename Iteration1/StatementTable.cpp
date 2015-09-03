#include "StatementTable.h"

StatementTable::StatementTable () {

}

Node* StatementTable::getStatement(int statementNum) {
	return table[statementNum];
}

std::vector<Node*> StatementTable::getStatements() {
	std::vector<Node*> nodes;
    for(std::map<int, Node*>::iterator it = table.begin(); it != table.end(); ++it) {
    	nodes.push_back( it->second );
    }
	return nodes;
}

std::vector<Node*> StatementTable::getStatements(synt_type st) {
	std::vector<Node*> nodes;
    for(std::map<int, Node*>::iterator it = table.begin(); it != table.end(); ++it) {
		Node* node = it->second;
		if (node->getType() == st) {
			nodes.push_back(node);
		}
    }
	return nodes;
}


void StatementTable::addStatement(int statementNum, Node* statementNode) {
	table[statementNum] = statementNode;
}