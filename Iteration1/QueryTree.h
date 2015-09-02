#ifndef QUERYTREE_H
#define QUERYTREE_H

#include "stdafx.h"
#include "QueryNode.h"
#include "PatternNode.h"

class QueryTree {

public:
	QueryTree ();
	QueryNode* getQuery();
	PatternNode* getPattern();

	void addQuery(QueryNode*);
	void addPattern(PatternNode*);
};

#endif