#include "stdafx.h"
#include "QueryPart.h"

using namespace std;

struct Query_Object {
	bool isValid;
	vector<ParamNode*> resultVec;
	vector<QueryPart*> queryVec;
};