#ifndef QUERYTREE_H
#define QUERYTREE_H

#include "stdafx.h"
#include "QueryNode.h"
#include "PatternNode.h"

class QueryTree {
	QueryNode* firstQuery;
	PatternNode* firstPattern;
	QueryNode* currQuery;
	PatternNode* currPattern;
	QueryNode* lastAddedQuery;
	PatternNode* lastAddedPattern;

public:
	QueryTree ();
	QueryNode* getQuery();
	PatternNode* getPattern();

	void addQuery(QueryNode*);
	void addPattern(PatternNode*);
};

#endif