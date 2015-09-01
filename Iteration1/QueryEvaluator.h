#ifndef QUERYEVALUATOR_H
#define QUERYEVALUATOR_H

#include "stdafx.h"
#include "querytree.h"
#include "symboltable.h"

class QueryEvaluator {

public:
	QueryTree (SymbolTable*, QueryTree*);
};

#endif