#include "stdafx.h"
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

int StatementTable::getStatementCount(synt_type st) {
	if ( count.find(st) == count.end() ) {
		return count[st];
	}
	return 0;
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
	if ( count.find(statementNode->getType()) == count.end() ) {
		count[statementNode->getType()] = 1; // magic number!!
	} else {
		int current = count[statementNode->getType()];
		count[statementNode->getType()] = current += 1; // magic number!!
	}
}