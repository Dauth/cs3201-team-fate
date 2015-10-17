// spa_build.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "QueryEvaluator.h"
#include "AST.h"
#include "SimpleParser.h"
#include "QueryParser.h"

int _tmain(int argc, _TCHAR* argv[]) {
	PKB pkb;
	ExpressionTree et = ExpressionTree(&pkb);
	AST ast = AST(&pkb, &et);
	SimpleParser sp = SimpleParser(&ast);
	DesignExtractor de = DesignExtractor(&pkb, &et);
	QueryEvaluator* qe = new QueryEvaluator(&pkb, &de);
	QueryParser qp = QueryParser();

	std::vector<std::string> r = sp.readSourceFile("C:\\Users\\sonclark\\Desktop\\simple.txt");
	std::vector<Node*> a = sp.execute(r);

	std::string input;
	std::getline(std::cin,input);

	std::list<std::string> result;
	
	while(input != "off") {
		QueryObject qObj = qp.getQueryObject(input);
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

