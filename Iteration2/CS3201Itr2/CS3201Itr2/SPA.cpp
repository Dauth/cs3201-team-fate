#include "stdafx.h"
#include "SPA.h"

SPA::SPA() {
	et = new ExpressionTree();
	pkb = new PKB(et);
	ast = new AST(pkb, et);
	sp = new SimpleParser(ast);
	qe = new QueryEvaluator(pkb);
	qp = new QueryParser();
}

void SPA::processSimpleSourceFile(std::string filepath) {
	std::vector<std::string> r = sp->readSourceFile(filepath);
	sp->execute(r);
	pkb->buildCFG();
}

std::list<std::string> SPA::executeQuery(std::string pql) {
	Query_Object qObj = qp->getQueryObject(pql);
	return qe->evaluate(qObj.resultVec, qObj.queryVec);
}

void SPA::startCommandLinePQLShell() {
	std::string input;
	std::getline(std::cin,input);

	std::list<std::string> result;
	
	while(input != "off") {
		Query_Object qObj = qp->getQueryObject(input);
		result = qe->evaluate(qObj.resultVec, qObj.queryVec);
		if(!result.empty()){
			std::list<std::string>::iterator i = result.begin();
			std::cout<<(*i);
			for(i++; i!=result.end(); i++) {
				std::cout<<", "<<(*i);
			}
		}
		std::cout<<"\n";
		std::getline(std::cin,input);
	}
}