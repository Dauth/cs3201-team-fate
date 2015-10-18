#ifndef QUERYPART_H
#define QUERYPART_H

#include "stdafx.h"
#include "QueryType.h"
#include "ParamNode.h"

class QueryPart {
	QueryType queryType;
	ParamNode* leftParam;
	ParamNode* rightParam;
	ParamNode* lastParam;

public:
	QueryPart(QueryType, ParamNode*, ParamNode*, ParamNode*);
	QueryType getType();
	ParamNode* getLeftParam();
	ParamNode* getRightParam();
	ParamNode* getLastParam();
};

#endif