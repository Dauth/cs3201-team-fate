#ifndef USESTABLE_H
#define USESTABLE_H

#include "stdafx.h"
#include "Node.h"

class UsesTable {
	// Statements mapped to Variables
	std::unordered_map<Node*, std::vector<Node*>> table;
	
public:
	UsesTable();
	std::vector<Node*> getUsers();
	std::vector<Node*> getUsers(synt_type);
	std::vector<Node*> getUsed(Node*);
	
	void addUsedBy(Node*, Node*);
};

#endif