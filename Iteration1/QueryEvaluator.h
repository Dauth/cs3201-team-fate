#ifndef QUERYEVALUATOR_H
#define QUERYEVALUATOR_H

#include "stdafx.h"
#include "QueryTree.h"
#include "Symbol.h"
#include "PKB.h"
#include "DesignExtractor.h"

class QueryEvaluator {
	Symbol* symbol;
	QueryTree* tree;
	bool hasResult;
	PKB* pkb;
	DesignExtractor* dEx;

	void evaluatePattern();
	void evaluateQuery();
	std::vector<Node*> evaluateLeft(query_type, ParamNode*);
	std::vector<Node*> evaluateRight(query_type, ParamNode*);
	std::vector<Node*> evaluateLeftByType(query_type, int);
	std::vector<Node*> evaluateLeftByType(query_type, std::string);
	std::vector<Node*> evaluateLeftByType(query_type, synt_type);
	std::vector<Node*> evaluateRightByType(query_type, int);
	std::vector<Node*> evaluateRightByType(query_type, std::string);
	std::vector<Node*> evaluateRightByType(query_type, synt_type);
	void evaluateResult();
	ParamNode* getOptimal(ParamNode*, ParamNode*);
	bool resultNotEmpty(ParamNode*, std::vector<Node*>);

public:
	QueryEvaluator (PKB*, DesignExtractor*);
	void evaluate(Symbol*, QueryTree*);
};

#endif