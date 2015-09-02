#ifndef QUERYNODE_H
#define QUERYNODE_H

#include "stdafx.h"
#include "query_type.h"
#include "ParamNode.h"

class QueryNode {

public:
	QueryNode (query_type, ParamNode*, ParamNode*);
	query_type getType();
	QueryNode* getNext();
	ParamNode* getLeftParam();
	ParamNode* getRightParam();

	void setNext(QueryNode*);
};

#endif