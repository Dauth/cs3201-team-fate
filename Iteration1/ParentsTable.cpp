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

std::vector<Node*> ParentsTable::getChild(Node* node) {
	if ( table.find(node) == table.end() ) {
		return std::vector<Node*>();
	}
	return table[node];
}

std::vector<Node*> ParentsTable::getChildren(synt_type st) {
	std::vector<Node*> allChildren;
	for(std::map<Node*, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); it++) {
		std::vector<Node*> children = it->second;
		for(int i=0; i<children.size(); i++) {
			if(children[i]->getType() == st) {
				allChildren.push_back(children[i]);
			}
		}
	}
	return allChildren;
}

void ParentsTable::addChild(Node* nodeLeft, Node* nodeRight) {
	if ( table.find(nodeLeft) == table.end() ) {
		std::vector<Node*> nodes;
		table[nodeLeft] = nodes;
	}
	table[nodeLeft].push_back(nodeRight);
}