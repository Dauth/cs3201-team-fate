#ifndef FOLLOWSFUNCTIONS_H
#define FOLLOWSFUNCTIONS_H

#include "stdafx.h"
#include "Node.h"
#include "PKB.h"

class FollowsFunctions{
	PKB* pkb;

public:
	FollowsFunctions(PKB*);
	std::vector<Node*> getFollowingStar(int statementLine);
	std::vector<Node*> getFollowingStar(synt_type type);
	std::vector<Node*> getFollowedByStar(int statementLine);
	std::vector<Node*> getFollowedByStar(synt_type type);
};
#endif