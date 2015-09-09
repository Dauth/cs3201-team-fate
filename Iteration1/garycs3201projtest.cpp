// spa_build.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include "stdafx.h"
#include "QueryEvaluator.h"
#include "AST.h"
#include "SimpleParser.h"

int _tmain(int argc, _TCHAR* argv[]) {
	PKB pkb;
	ExpressionTree et = ExpressionTree(&pkb);
	AST ast = AST(&pkb, &et);
	SimpleParser sp = SimpleParser(&ast);
	DesignExtractor de = DesignExtractor(&pkb, &et);
	QueryEvaluator qe = QueryEvaluator(&pkb, &de);

	//std::vector<std::string> r = sp.readSourceFile("C:\\Users\\temporal\\Documents\\simpletest.txt");
	//std::vector<Node*> a = sp.execute(r);


	//a=v+x*y+z*t
	std::string inflix = "v+x*y+z*t";//  
	std::vector<char> v = et.expressionConverter(inflix);

	Node* tNode = et.exptreeSetupSON(v);




	Node* proc = pkb.createProcedure("test");
	Node* testStmtLst1 = pkb.createNode(statementList, 1);
	proc->setLeftChild(testStmtLst1);

	Node* assignStmNode = pkb.createNode(assignment, 1, "", nullptr, nullptr, nullptr, proc);
	Node* assignStmNode_LEFT = pkb.createNode(variable, 1, "a", nullptr, assignStmNode, nullptr, proc);
	Node* assignStmNode_RIGHT = et.exptreeSetup(v, 1, assignStmNode, proc, nullptr);

	assignStmNode->setLeftChild(assignStmNode_LEFT);
	assignStmNode->setRightChild(assignStmNode_RIGHT);
	testStmtLst1->addStmt(assignStmNode);


	int i11 = 0;
	system("pause");	
}

