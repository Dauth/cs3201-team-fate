#ifndef VARIABLE_H
#define VARIABLE_H

#include "stdafx.h"

class Node;

class Variable {
	std::string name;
	std::vector<Node*> usedBy;
	std::vector<Node*> modifiedBy;
	
public:
	Variable(std::string);
	std::string getName();
	std::vector<Node*> getUsedBy();
	std::vector<Node*> getModifiedBy();

	void addUsedBy(Node*);
	void addModifiedBy(Node*);
};

#endif