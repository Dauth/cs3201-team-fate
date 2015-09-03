#include "ParentsTable.h"

ParentsTable::ParentsTable () {

}

std::vector<Node*> ParentsTable::getParents() {
	std::vector<Node*> parents;
	for(std::map<Node*, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); it++) {
		parents.push_back(it->first);
	}
}

std::vector<Node*> ParentsTable::getChildren(Node* node) {
	std::vector<Node*> children = table[node];
	return children;
}

void ParentsTable::addChild(Node* nodeLeft, Node* nodeRight) {
	if ( table.find(nodeLeft) == table.end() ) {
		std::vector<Node*> nodes;
		table[nodeLeft] = nodes;
	}
	table[nodeLeft].push_back(nodeRight);
}