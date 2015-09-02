#include "UsesTable.h"

UsesTable::UsesTable () {

}

std::vector<Node*> UsesTable::getUsers() {
	std::vector<Node*> users;
	for(std::map<Node*, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); it++) {
		users.push_back(it->first);
	}
}

std::vector<Node*> UsesTable::getUsed(Node* pnode) {
	std::vector<Node*> used = table[pnode];
	return used;
}

void UsesTable::addUsed(Node* pnodeLeft, Node* pnodeRight) {
	if ( table.find(pnodeLeft) == table.end() ) {
		std::vector<Node*> pnodes;
		table[pnodeLeft] = pnodes;
	}
	table[pnodeLeft].push_back(pnodeRight);
}