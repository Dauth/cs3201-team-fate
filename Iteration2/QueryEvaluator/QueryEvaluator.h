#ifndef QUERYEVALUATOR_H
#define QUERYEVALUATOR_H

#include "stdafx.h"
#include "QueryPart.h"
#include "SynonymValues.h"
#include "PKB.h"

using namespace std;

class QueryEvaluator {
	PKB* pkb;
	bool hasResult;
	vector<ParamNode*> resultSynonyms;
	vector<QueryPart*> queryParts;
	vector<QueryPart*> queryWithNoResult;
	vector<QueryPart*> queryWithOneResult;
	vector<QueryPart*> queryWithTwoResults;
	vector<SynonymValues*> synonymVec;
	vector<vector<pair<string, string>>> resultTuples;
	list<string> finalResult;

	void optimise();
	void sortQueryParts();
	void orderQueryParts(vector<QueryPart*>*);
	void evalQueryWithNoResult();
	void evalQueryWithOneResult();
	void evalQueryWithTwoResults();
	vector<pair<string, string>> evalWithQuery(QueryPart*);
	vector<pair<string, string>> getResult(QueryPart*);
	vector<pair<string, string>> getResultFromPKB(QueryType, string, string);
	vector<pair<string, string>> getResultFromPKB(QueryType, string, SyntType);
	vector<pair<string, string>> getResultFromPKB(QueryType, SyntType, string);
	vector<pair<string, string>> getResultFromPKB(QueryType, SyntType, SyntType);
	SyntType getSyntType(QueryType);
	void updateSynVal(ParamNode*, ParamNode*, vector<pair<string, string>>);
	void QueryEvaluator::updateTwoSynVal(ParamNode*, ParamNode*, vector<pair<string, string>>);
	void QueryEvaluator::updateLeftSynVal(ParamNode*, ParamNode*, vector<pair<string, string>>);
	void QueryEvaluator::updateRightSynVal(ParamNode*, ParamNode*, vector<pair<string, string>>);
	void updateRelatedSynVal(SynonymValues*);
	void evalFinalResult();
	void formFinalResult(vector<string>);
	bool existsInSynVec(string);
	SynonymValues* getSynVal(string);

public:
	QueryEvaluator(PKB*);
	list<string> evaluate(vector<ParamNode*>, vector<QueryPart*>);
};

#endif