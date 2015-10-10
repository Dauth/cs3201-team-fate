#ifndef VARIABLETABLE_H
#define VARIABLETABLE_H

#include "stdafx.h"
#include "Node.h"
#include "Variable.h"

class VariableTable {
	std::unordered_map<std::string, Variable*> table;
	
public:
	VariableTable();
	Variable* getOrCreateVariable(std::string);
	std::vector<Node*> getVariableNodes();
	int getVariableCount();
	int getVariableCount(std::string);
	std::vector<Node*> getModifiedBy(std::string);
	std::vector<Node*> getUsedBy(std::string);

	void addModifiedBy(std::string, Node*);
	void addUsedBy(std::string, Node*);
	void addNode(std::string, Node*);
};

#endif