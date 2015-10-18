#ifndef CFG_H
#define CFGN_H

#include "stdafx.h"
#include "CFGNode.h"
#include "Node.h"

using namespace std;

class CFG{
	std::vector<CFGNode*> cfgs;
	std::vector<pair<string, string>> pairs;

	CFGNode* createNorm(Node*);
	CFGNode* createWhile(Node*);
	CFGNode* createIfLeft(CFGNode*);
	CFGNode* createIfRight(CFGNode*);
	CFGNode* createNext(Node*);
	void linkNode(CFGNode*, CFGNode*);
	void createAndAddPair(CFGNode*,CFGNode*);

public:
	CFG();
	void buildCFG(Node*);
	vector<pair<string, string>> getAllPairs();
	CFGNode* getCFG(std::string);
};


#endif