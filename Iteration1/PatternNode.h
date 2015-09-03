#ifndef PATTERNNODE_H
#define PATTERNNODE_H

#include "stdafx.h"
#include "ParamNode.h"

class PatternNode {
	ParamNode* leftParam;
	ParamNode* middleParam;
	ParamNode* rightParam;
	PatternNode* nextPattern;

public:
	PatternNode (ParamNode*, ParamNode*, ParamNode*);
	PatternNode* getNext();
	ParamNode* getLeftParam();
	ParamNode* getMiddleParam();
	ParamNode* getRightParam();

	void setNext(PatternNode*);
};

#endif