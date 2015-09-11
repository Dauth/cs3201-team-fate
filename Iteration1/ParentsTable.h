#ifndef PARENTSTABLE_H
#define PARENTSTABLE_H

#include "stdafx.h"
#include "Node.h"

class ParentsTable {
	std::unordered_map<Node*, std::vector<Node*>> table;
	
public:
	ParentsTable();
	std::vector<Node*> getParents();
	std::vector<Node*> getChild(Node*);
	std::vector<Node*> getChildren(synt_type);
	void addChild(Node*, Node*);
};

#endif