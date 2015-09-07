#ifndef PROCEDURETABLE_H
#define PROCEDURETABLE_H

#include "stdafx.h"
#include "Node.h"

class ProcedureTable {
	std::map<std::string, Node*> table;
	
public:
	ProcedureTable();
	Node* createProcedure(std::string);
	Node* getProcedure(std::string);
};

#endif