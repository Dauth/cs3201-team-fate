#include "stdafx.h"
#include "StatementTable.h"

const int ONE = 1;
// statement table keeps track of all statements in the simple source program
// allows retrieval of statement nodes based on statement number or statement type
StatementTable::StatementTable () {
	count[statement] = 0;
}

// returns a statement node based on its statement number
Node* StatementTable::getStatement(std::string statementNum) {
	if ( table.find(statementNum) == table.end() ) {
		return nullptr;
	}
	return table[statementNum];
}

// returns all statements
std::vector<Node*> StatementTable::getStatements() {
	std::vector<Node*> nodes;
    for(std::map<std::string, Node*>::iterator it = table.begin(); it != table.end(); ++it) {
    	nodes.push_back( it->second );
    }
	return nodes;
}

// returns the number of statements of a particular type
int StatementTable::getStatementCount(SyntType st) {
	if ( count.find(st) == count.end() ) {
		return 0;
	}
	return count[st];
}

// gets statement nodes of a particular statement type
std::vector<Node*> StatementTable::getStatements(SyntType st) {
	std::vector<Node*> nodes;
    for(std::map<std::string, Node*>::iterator it = table.begin(); it != table.end(); ++it) {
		Node* node = it->second;
		SyntType nt = node->getType();
		if (nt == st || (st == statement && (node->isStatement()))) {
			nodes.push_back(node);
		} else if (st == statementList) {
			if(node->getParent()->getIndexLst()[node] == ONE) {
				nodes.push_back(node);
			}
		}
    }
	return nodes;
}

// adds a statement to the statement table
void StatementTable::addStatement(std::string statementNum, Node* statementNode) {
	table[statementNum] = statementNode;
	if ( count.find(statementNode->getType()) == count.end() ) {
		count[statementNode->getType()] = ONE; 
	} else {
		int current = count[statementNode->getType()];
		count[statementNode->getType()] = current + ONE;
	}
	count[statement] = count[statement] + ONE;
}