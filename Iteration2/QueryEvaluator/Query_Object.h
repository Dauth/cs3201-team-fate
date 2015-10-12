#include "stdafx.h"
#include "QueryPart.h"

struct Query_Object {
	std::vector<ParamNode*> resultVec;
	std::vector<QueryPart*> queryVec;
};