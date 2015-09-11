#ifndef MODIFIESTABLE_H
#define MODIFIESTABLE_H

#include "stdafx.h"
#include "Node.h"

class ModifiesTable {
	std::unordered_map<Node*, std::vector<Node*>> table;
	
public:
	ModifiesTable();
	std::vector<Node*> getModifiers();
	std::vector<Node*> getModifiers(synt_type);
	std::vector<Node*> getModified(Node*);
	
	void addModifies(Node*, Node*);
};

#endif