#ifndef QUERYEVALUATOR_H
#define QUERYEVALUATOR_H

#include "stdafx.h"
#include "QueryTree.h"
#include "Symbol.h"
#include "PKB.h"
#include "DesignExtractor.h"
#include "SymbolConnection.h"

class QueryEvaluator {
	Symbol* symbol;
	QueryTree* tree;
	bool hasResult;
	PKB* pkb;
	DesignExtractor* dEx;
	SymbolConnection sc;

	void evaluatePattern();
	void evaluateQuery();
	void evaluateSinglePattern(PatternNode*);
	void evaluateSingleQuery(QueryNode*);
	bool evaluateLeft(ParamNode*, query_type, ParamNode*);
	bool evaluateRight(ParamNode*, query_type, ParamNode*);
	std::vector<Node*> evaluateLeftByType(query_type, int);
	std::vector<Node*> evaluateLeftByType(query_type, std::string);
	std::vector<Node*> evaluateLeftByType(query_type, synt_type);
	std::vector<Node*> evaluateRightByType(query_type, int);
	std::vector<Node*> evaluateRightByType(query_type, std::string);
	std::vector<Node*> evaluateRightByType(query_type, synt_type);
	std::vector<Node*> getVarFromPattern(std::vector<Node*>);
	void evaluateResult();
	ParamNode* getOptimal(ParamNode*, ParamNode*);
	bool resultNotEmpty(ParamNode*, std::vector<Node*>);
	std::string getStringResult(Data*);
	void changePartners(ParamNode*);

public:
	QueryEvaluator (PKB*, DesignExtractor*);
	void evaluate(Symbol*, QueryTree*);
};

#endif