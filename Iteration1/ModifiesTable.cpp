#include "ModifiesTable.h"

ModifiesTable::ModifiesTable () {

}

std::vector<Node*> ModifiesTable::getModifiers() {
	std::vector<Node*> modifiers;
	for(std::map<Node*, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); ++it) {
		modifiers.push_back(it->first);
	}
	return modifiers;
}

std::vector<Node*> ModifiesTable::getModifiers(synt_type st) {
	std::vector<Node*> modifiers;
	for(std::map<Node*, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); ++it) {
		Node *node = it->first;
		if (node->getType() == st) {
			modifiers.push_back(node);
		}
    }
	return modifiers;
}

std::vector<Node*> ModifiesTable::getModified(Node* node) {
	std::vector<Node*> modified = table[node];
	return modified;
}

void ModifiesTable::addModifies(Node* nodeLeft, Node* nodeRight) {
	if ( table.find(nodeLeft) == table.end() ) {
		std::vector<Node*> nodes;
		table[nodeLeft] = nodes;
	}
	table[nodeLeft].push_back(nodeRight);
}