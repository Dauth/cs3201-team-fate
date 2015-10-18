#ifndef SPA_H
#define SPA_H

#include "QueryEvaluator.h"
#include "AST.h"
#include "SimpleParser.h"
#include "QueryParser.h"

class SPA {
	PKB* pkb;
	ExpressionTree* et;
	AST* ast;
	SimpleParser* sp;
	QueryEvaluator* qe;
	QueryParser* qp;

public:
	SPA();
	void processSimpleSourceFile(std::string);
	std::list<std::string> executeQuery(std::string);
	void startCommandLinePQLShell();
};


#endif