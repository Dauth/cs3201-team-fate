#include "ParentsTable.h"

ParentsTable::ParentsTable () {

}

std::vector<Node*> ParentsTable::getParents() {
	std::vector<Node*> parents;
	for(std::map<Node*, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); it++) {
		parents.push_back(it->first);
	}
}

std::vector<Node*> ParentsTable::getChildren(Node* pnode) {
	std::vector<Node*> children = table[pnode];
	return children;
}

void ParentsTable::addChild(Node* pnodeLeft, Node* pnodeRight) {
	if ( table.find(pnodeLeft) == table.end() ) {
		std::vector<Node*> pnodes;
		table[pnodeLeft] = pnodes;
	}
	table[pnodeLeft].push_back(pnodeRight);
}