#include "UsesTable.h"
#include "stdafx.h"

UsesTable::UsesTable () {

}

std::vector<Node*> UsesTable::getUsers() {
	std::vector<Node*> users;
	for(std::map<Node*, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); it++) {
		users.push_back(it->first);
	}
	return users;
}

std::vector<Node*> UsesTable::getUsers(synt_type st) {
	std::vector<Node*> users;
	for(std::map<Node*, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); ++it) {
		Node* node = it->first;
		if (node->getType() == st) {
			users.push_back(node);
		}
    }
	return users;
}

std::vector<Node*> UsesTable::getUsed(Node* node) {
	std::vector<Node*> used = table[node];
	return used;
}

void UsesTable::addUsedBy(Node* nodeLeft, Node* nodeRight) {
	if ( table.find(nodeLeft) == table.end() ) {
		std::vector<Node*> nodes;
		table[nodeLeft] = nodes;
	}
	table[nodeLeft].push_back(nodeRight);
}