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

int StatementTable::getStatementCount(SyntType st) {
	if ( count.find(st) == count.end() ) {
		return 0;
	}
	return count[st];
}

std::vector<Node*> StatementTable::getStatements(SyntType st) {
	std::vector<Node*> nodes;
    for(std::map<std::string, Node*>::iterator it = table.begin(); it != table.end(); ++it) {
		Node* node = it->second;
		SyntType nt = node->getType();
		if (nt == st || (st == statement && (node->isStatement()))) {
			nodes.push_back(node);
		} else if (st == statementList) {
			if(node->getParent()->getIndexLst()[node] == 1) {
				nodes.push_back(node);
			}
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