#ifndef AST_H
#define AST_H

#include "stdafx.h"
#include "Node.h"
#include "PKB.h"
#include "ExpressionTree.h"
#include <string>
#include "Twin.h"
#include <stack>

class AST {
	PKB* pkb;
	ExpressionTree* expTree;
private:
	int AST::getStatementType(std::string);
	std::string AST::extractStatementPart(int, std::string);
	int AST::getNumOfClosingbraces(std::string);

public:
	AST(PKB*, ExpressionTree*);
	void catchProcedureInceptionException(std::stack<std::string>&, unsigned, int);
	void catchDupProcedureException(unsigned i, Node* procStm);
	void catchRecursiveCallException(std::string& currentProcName, unsigned i, std::string& callProcName);
	void catchUnbalancedInfixException(unsigned i, bool isInflixBalance);
	std::vector<Node*> AST::buildAST(std::vector<std::string>);
};

#endif