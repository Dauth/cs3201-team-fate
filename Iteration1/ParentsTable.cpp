#include "stdafx.h"
#include "ParentsTable.h"

ParentsTable::ParentsTable () {

}

std::vector<Node*> ParentsTable::getParents() {
	std::vector<Node*> parents;
	for(std::map<Node*, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); it++) {
		parents.push_back(it->first);
	}
	return parents;
}

std::vector<Node*> ParentsTable::getChildren(Node* node) {
	if ( table.find(node) == table.end() ) {
		return std::vector<Node*>();
	}
	return table[node];
}

void ParentsTable::addChild(Node* nodeLeft, Node* nodeRight) {
	if ( table.find(nodeLeft) == table.end() ) {
		std::vector<Node*> nodes;
		table[nodeLeft] = nodes;
	}
	table[nodeLeft].push_back(nodeRight);
}