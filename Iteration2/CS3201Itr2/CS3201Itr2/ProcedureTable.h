#ifndef PROCEDURETABLE_H
#define PROCEDURETABLE_H

#include "stdafx.h"
#include "Node.h"

class ProcedureTable {
	
	public:

		ProcedureTable();
		Node* createProcedure(std::string);
		Node* getProcedure(std::string);
		std::vector<Node*> getAllProcedures();

	private:

		std::unordered_map<std::string, Node*> table;
};

#endif