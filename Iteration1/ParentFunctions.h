#ifndef PARENTFUNCTIONS_H
#define PARENTFUNCTIONS_H

#include "stdafx.h"
#include "Node.h"

public:
	std::vector<Node*> getParentsFor(Node* parent);
	Node* getParentsOf(Node* child);
	std::vector<Node*> getParentsStarFor(Node* parent);
	std::vector<Node*> getParentsStarOf(Node* child);
};

#endif