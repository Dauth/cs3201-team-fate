#ifndef MODIFIES_H
#define MODIFIES_H

#include "stdafx.h"
#include "Node.h"

class Modifies {
	Node* modifier;
	std::vector<Node*> modifiedList;
	
public:
	Modifies(Node*);
	std::vector<Node*> getModifier();
	std::vector<Node*> getModifiedList();
}

#endif