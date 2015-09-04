#ifndef FollowsFunctions_H
#define FollowsFunctions_H

#include "stdafx.h"
#include "Node.h"

class FollowsFunctions{

public:
	FollowsFunctions();
	std::vector<Node*> getFollowingStar(int statementLine);
	std::vector<Node*> getFollowingStar(synt_type type);
	std::vector<Node*> getFollowedByStar(int statementLine);
	std::vector<Node*> getFollowedByStar(synt_type type);
};
#endif