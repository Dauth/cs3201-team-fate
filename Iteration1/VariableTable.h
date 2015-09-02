#ifndef VARIABLETABLE_H
#define VARIABLETABLE_H

#include "stdafx.h"
#include "Node.h"
#include "Variable.h"

class VariableTable {
	std::map<std::string, Variable*> table;
	
public:
	VariableTable();
	Variable* getVariable(std::string);
	std::vector<Node*> getModifiedBy(std::string);
	std::vector<Node*> getUsedBy(std::string);

	void addVariable(std::string, Node*);
	void addModifiedBy(std::string, Node*);
	void addUsedBy(std::string, Node*);
};

#endif