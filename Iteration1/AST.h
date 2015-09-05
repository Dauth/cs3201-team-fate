#ifndef AST_H
#define AST_H

#include "stdafx.h"
#include "Node.h"
#include "PKB.h"
#include<string>


class AST {
	PKB pkb;
	ExpressionTree expTree;

	int AST::getStatementType(std::string);
	std::string AST::extractStatementPart(int, std::string);
	int AST::getNumOfClosingbraces(std::string);

public:
	std::vector<Node*> AST::buildAST(std::vector<std::string>);

};

#endif