#ifndef VARIABLETABLE_H
#define VARIABLETABLE_H

#include "stdafx.h"
#include "Node.h"

class VariableTable {
	std::unordered_map<std::string, std::vector<Node*>> table;
	
public:
	VariableTable();
	std::vector<Node*> getNode(std::string);
	std::vector<Node*> getNodes();
	int getVariableCount();
	int getVariableCount(std::string);

	void addNode(Node*);
};

#endif