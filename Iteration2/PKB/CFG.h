#ifndef CFG_H
#define CFGN_H

#include "stdafx.h"
#include "CFGNode.h"

class CFG{
	std::vector<CFGNode*> cfgs;

	CFGNode* createNorm(Node*);
	CFGNode* createWhile(Node*);
	CFGNode* createIfLeft(CFGNode*);
	CFGNode* createIfRight(CFGNode*);
	CFGNode* createNext(Node*);

public:
	void buildCFG(Node*);
	CFGNode* getCFG(std::string);
}


#endif