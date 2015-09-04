#ifndef PARENTFUNCTIONS_H
#define PARENTFUNCTIONS_H


#include "stdafx.h"
#include "Node.h"
#include "PKB.h"

class ParentFunctions{
	PKB* pkb;

public:
	ParentFunctions(PKB*);
	std::vector<Node*> getChildrenStar(int);
	std::vector<Node*> getChildrenStar(synt_type);
	std::vector<Node*> getParentsStar(int);
	std::vector<Node*> getParentsStar(synt_type);
};

#endif