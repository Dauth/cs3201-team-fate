#ifndef PROCEDURETABLE_H
#define PROCEDURETABLE_H

#include "stdafx.h"
#include "AST.h"

class ProcedureTable {
	std::map<std::string, AST> table;
	
public:
	ProcedureTable();
	AST* getOrCreateProcedure(std::string);
};

#endif