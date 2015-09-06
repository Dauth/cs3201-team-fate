#ifndef DESIGNEXTRACTOR_H
#define DESIGNEXTRACTOR_H
#include "stdafx.h"
#include "Node.h"
#include "PKB.h"
#include "ExpressionTree.h"

class DesignExtractor{
	PKB* pkb;
	ExpressionTree* et;
	int compare(Node*, Node*);
public:
	DesignExtractor(PKB*,ExpressionTree*);

	std::vector<Node*> getFollowingStar(int statementLine);
	std::vector<Node*> getFollowingStar(synt_type type);
	std::vector<Node*> getFollowedByStar(int statementLine);
	std::vector<Node*> getFollowedByStar(synt_type type);

	std::vector<Node*> getChildrenStar(int);
	std::vector<Node*> getChildrenStar(synt_type);
	std::vector<Node*> getParentsStar(int);
	std::vector<Node*> getParentsStar(synt_type);

	std::vector<Node*> searchWithPattern(synt_type,std::string,std::string);
};
	

#endif