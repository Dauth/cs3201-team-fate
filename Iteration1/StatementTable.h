#ifndef STATEMENTTABLE_H
#define STATEMENTTABLE_H

#include "stdafx.h"
#include "Node.h"

class StatementTable {
	std::map<int, Node*> table;
	std::map<synt_type, int> count;
	
public:
	StatementTable();
	Node* getStatement(int);
	std::vector<Node*> getStatements();
	int getStatementCount(synt_type);
	std::vector<Node*> getStatements(synt_type);

	void addStatement(int, Node*);
};

#endif