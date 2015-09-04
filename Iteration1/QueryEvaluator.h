#ifndef QUERYEVALUATOR_H
#define QUERYEVALUATOR_H

#include "stdafx.h"
#include "QueryTree.h"
#include "SymbolTable.h"

class QueryEvaluator {
	QueryTree* tree;
	SymbolTable* table;
	bool hasResult;

	void evaluatePattern();
	void evaluateQuery();
	void evaluateResult();

public:
	QueryEvaluator ();
	void evaluate(SymbolTable*, QueryTree*);
};

#endif