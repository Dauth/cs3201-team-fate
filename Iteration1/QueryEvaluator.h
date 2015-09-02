#ifndef QUERYEVALUATOR_H
#define QUERYEVALUATOR_H

#include "stdafx.h"
#include "QueryTree.h"
#include "SymbolTable.h"

class QueryEvaluator {

public:
	QueryTree (SymbolTable*, QueryTree*);
};

#endif