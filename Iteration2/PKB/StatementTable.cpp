#include "stdafx.h"
#include "StatementTable.h"

StatementTable::StatementTable () {
	count[statement] = 0;
}

Node* StatementTable::getStatement(std::string statementNum) {
	if ( table.find(statementNum) == table.end() ) {
		return nullptr;
	}
	return table[statementNum];
}

std::vector<Node*> StatementTable::getStatements() {
	std::vector<Node*> nodes;
    for(std::map<std::string, Node*>::iterator it = table.begin(); it != table.end(); ++it) {
    	nodes.push_back( it->second );
    }
	return nodes;
}

int StatementTable::getStatementCount(synt_type st) {
	if ( count.find(st) == count.end() ) {
		return 0;
	}
	return count[st];
}

std::vector<Node*> StatementTable::getStatements(synt_type st) {
	std::vector<Node*> nodes;
    for(std::map<std::string, Node*>::iterator it = table.begin(); it != table.end(); ++it) {
		Node* node = it->second;
		synt_type nt = node->getType();
		if (nt == st || (st == statement && (nt == assignment || nt == ifelse || nt == whileLoop))) {
			nodes.push_back(node);
		}
    }
	return nodes;
}


void StatementTable::addStatement(std::string statementNum, Node* statementNode) {
	table[statementNum] = statementNode;
	if ( count.find(statementNode->getType()) == count.end() ) {
		count[statementNode->getType()] = 1; // magic number!!
	} else {
		int current = count[statementNode->getType()];
		count[statementNode->getType()] = current + 1; // magic number!!
	}
	count[statement] = count[statement] + 1;
}