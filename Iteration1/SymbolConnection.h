#ifndef SYMBOLCONNECTION_H
#define SYMBOLCONNECTION_H

#include "stdafx.h"
#include "QueryNode.h"
#include "PatternNode.h"

class SymbolConnection{
	std::vector<PatternNode*> pVec;
	std::vector<QueryNode*> qVec;

public:
	SymbolConnection();
	std::vector<PatternNode*> getPatterns(std::string);
	std::vector<QueryNode*> getQueries(std::string);
	void addPattern(PatternNode*);
	void addQuery(QueryNode*);
};

#endif