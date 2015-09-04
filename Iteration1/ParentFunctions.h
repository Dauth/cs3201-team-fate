#ifndef PARENTFUNCTIONS_H
#define PARENTFUNCTIONS_H


#include "stdafx.h"
#include "Node.h"
class ParentFunctions{
public:
	ParentFunctions();
	std::vector<Node*> getChildrenStar(int statementLine);
	std::vector<Node*> getChildrenStar(synt_type type);
	std::vector<Node*> getParentsStar(int statementLine);
	std::vector<Node*> getParentsStar(synt_type type);
};

#endif