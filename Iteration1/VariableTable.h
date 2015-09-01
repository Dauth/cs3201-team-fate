#ifndef VARIABLETABLE_H
#define VARIABLETABLE_H

#include "stdafx.h"
#include "Node.h"
#include "Variable.h"

class VariableTable {
	std::map<std::string, Variable*> table;
	
public:
	Variable();
	Variable* getVariable(std::string);
	Node* getModifiedBy(std::string);
	Node* getUsedBy(std::string);

	void addVariable(std::string, Node*);
	void addModifiedBy(std::string, Node*);
	void addUsedBy(std::string, Node*);
}

#endif