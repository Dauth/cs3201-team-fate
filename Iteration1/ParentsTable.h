#ifndef PARENTSTABLE_H
#define PARENTSTABLE_H

#include "stdafx.h"
#include "Node.h"

class ParentsTable {
	std::map<Node*, std::vector<Node*>> table;
	
public:
	ParentsTable();
	std::vector<Node*> getParents();
	std::vector<Node*> getChildren(Node*);
	
	void addParent(Node*);
	void addChild(Node*, Node*);
}

#endif