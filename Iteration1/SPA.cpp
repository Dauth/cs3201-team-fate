#include "stdafx.h"
#include "SPA.h"

SPA::SPA() {
	pkb = new PKB();
	et = new ExpressionTree(pkb);
	ast = new AST(pkb, et);
	sp = new SimpleParser(ast);
	de = new DesignExtractor(pkb, et);
	qe = new QueryEvaluator(pkb, de);
	qp = new QueryParser();
}

void SPA::processSimpleSourceFile(std::string filepath) {
	std::vector<std::string> r = sp->readSourceFile(filepath);
	std::vector<Node*> a = sp->execute(r);
}

std::list<std::string> SPA::executeQuery(std::string pql) {
	QueryObject qObj = qp->getQueryObject(pql);
	return qe->evaluate(qObj.symbol, qObj.tree);
}

void SPA::startCommandLinePQLShell() {
	std::string input;
	std::getline(std::cin,input);

	std::list<std::string> result;
	
	while(input != "off") {
		QueryObject qObj = qp->getQueryObject(input);
		result = qe->evaluate(qObj.symbol, qObj.tree);
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