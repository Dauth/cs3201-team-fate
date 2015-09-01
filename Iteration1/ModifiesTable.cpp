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

void ModifiesTable::addModifier(Node* pnode) {
	if ( table.find(pnode) == table.end() ) {
		std::vector<Node*> pnodes;
		table[pnode] = pnodes;
	}
}

void ModifiesTable::addModifies(Node* pnodeLeft, Node* pnodeRight) {
	table[pnodeLeft].push_back(pnodeRight);
}