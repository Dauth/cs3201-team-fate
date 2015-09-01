#ifndef QUERYTREE_H
#define QUERYTREE_H

#include "stdafx.h"
#include "querynode.h"

class QueryTree {

public:
	QueryTree ();
	QueryNode* getQueryPart();
	QueryNode* getPattern();

	void setQueryPart();
	void setPattern();
};

#endif