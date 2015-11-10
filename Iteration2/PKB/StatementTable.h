#ifndef STATEMENTTABLE_H
#define STATEMENTTABLE_H

#include "stdafx.h"
#include "Node.h"

class StatementTable {
	
	public:

		StatementTable();
		Node* getStatement(std::string);
		std::vector<Node*> getStatements();
		int getStatementCount(SyntType);
		std::vector<Node*> getStatements(SyntType);
		void addStatement(std::string, Node*);

	private:

		std::map<std::string, Node*> table;
		std::map<SyntType, int> count;
};

#endif