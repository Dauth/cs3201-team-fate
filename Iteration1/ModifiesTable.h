#ifndef MODIFIESTABLE_H
#define MODIFIESTABLE_H

#include "stdafx.h"
#include "Node.h"

class ModifiesTable {
	std::map<Node*, std::vector<Node*>> table;
	
public:
	ModifiesTable();
	std::vector<Node*> getModifiers();
	std::vector<Node*> getModified(Node*);
	
	void addModifier(Node*);
	void addModifies(Node*, Node*);
};

#endif