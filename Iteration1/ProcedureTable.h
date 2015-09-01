#ifndef PROCEDURETABLE_H
#define PROCEDURETABLE_H

#include "stdafx.h"
#include "AST.h"

class ProcedureTable {
	std::map<std::string, AST> procedureTable;
	
public:
	ProcedureTable();
	AST getProcedure(std::string);

	void addProcedure(std::string, AST);
}

#endif