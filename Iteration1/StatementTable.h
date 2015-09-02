#ifndef STATEMENTTABLE_H
#define STATEMENTTABLE_H

#include "stdafx.h"
#include "Node.h"

class StatementTable {
	std::map<int, Node*> table;
	
public:
	StatementTable();
	Node* getStatement(int);
	
	void addStatement(int, Node*);
};

#endif