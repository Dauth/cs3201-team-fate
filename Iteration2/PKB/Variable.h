#ifndef VARIABLE_H
#define VARIABLE_H

#include "stdafx.h"

class Node;

class Variable {
	std::string name;
	std::set<Node*> usedBy;
	std::set<Node*> modifiedBy;
	std::vector<Node*> nodes;
	
public:
	Variable(std::string);
	std::string getName();
	std::vector<Node*> getUsedBy();
	std::vector<Node*> getModifiedBy();
	std::vector<Node*> getNodes();
	int getNodeCount();

	void addUsedBy(Node*);
	void addModifiedBy(Node*);
	void addNode(Node*);
};

#endif