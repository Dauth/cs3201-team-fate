#include "ModifiesTable.h"

ModifiesTable::ModifiesTable () {

}

std::vector<Node*> ModifiesTable::getModifiers() {
	std::vector<Node*> modifiers;
	for(std::map<Node*, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); it++) {
		modifiers.push_back(it->first);
	}
}

std::vector<Node*> ModifiesTable::getModified(Node* pnode) {
	std::vector<Node*> modified = table[pnode];
	return modified;
}

void ModifiesTable::addModifies(Node* pnodeLeft, Node* pnodeRight) {
	if ( table.find(pnodeLeft) == table.end() ) {
		std::vector<Node*> pnodes;
		table[pnodeLeft] = pnodes;
	}
	table[pnodeLeft].push_back(pnodeRight);
}