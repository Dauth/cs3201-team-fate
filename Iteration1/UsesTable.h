#ifndef USESTABLE_H
#define USESTABLE_H

#include "stdafx.h"
#include "Node.h"

class UsesTable {
	std::map<Node*, std::vector<Node*>> table;
	
public:
	UsesTable();
	std::vector<Node*> getUsers();
	std::vector<Node*> getUsed(Node*);
	
	void addUsed(Node*, Node*);
};

#endif