#ifndef QUERYEVALUATOR_H
#define QUERYEVALUATOR_H

#include "stdafx.h"
#include "QueryPart.h"
#include "SynonymValues.h"
#include "PKB.h"

class QueryEvaluator {
	PKB* pkb;
	bool hasResult;
	std::vector<ParamNode*> resultSynonyms;
	std::vector<QueryPart*> queryParts;
	std::vector<QueryPart*> queryWithNoResult;
	std::vector<QueryPart*> queryWithOneResult;
	std::vector<QueryPart*> queryWithTwoResults;
	std::vector<SynonymValues*> synonymVec;
	std::vector<std::vector<std::pair<std::string, std::string>>*> resultTuples;
	std::list<std::string> finalResult;

	void optimise();
	void sortQueryParts();
	void evalQueryWithNoResult();
	void evalQueryWithOneResult();
	void evalQueryWithTwoResults();
	std::vector<std::pair<std::string, std::string>> evalWithQuery(QueryPart*);
	std::vector<std::pair<std::string, std::string>> getResult(QueryPart*);
	std::vector<std::pair<std::string, std::string>> getResultFromPKB(QueryType, std::string, std::string);
	std::vector<std::pair<std::string, std::string>> getResultFromPKB(QueryType, std::string, SyntType);
	std::vector<std::pair<std::string, std::string>> getResultFromPKB(QueryType, SyntType, std::string);
	std::vector<std::pair<std::string, std::string>> getResultFromPKB(QueryType, SyntType, SyntType);
	void updateSynVal(ParamNode*, ParamNode*, std::vector<std::pair<std::string, std::string>>);
	void updateRelatedSynVal(SynonymValues*);
	void evalFinalResult();
	void formFinalResult(std::string, int);
	bool existsInSynVec(std::string);
	SynonymValues* getSynVal(std::string);

public:
	QueryEvaluator(PKB*);
	std::list<std::string> evaluate(std::vector<ParamNode*>, std::vector<QueryPart*>);
};

#endif