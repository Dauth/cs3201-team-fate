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
	int AST::getNumOfOpenbraces(std::string);

public:
	AST(PKB*, ExpressionTree*);
	void catchEmptyContainerException(std::vector<Twin*>& twinVector, unsigned i);
	void setupWhileVarListNode(int lineNumber, std::vector<Twin*>& twinVector, std::string varName, Node*& whileStm, Node* procNode, Node*& whileStmLst);
	void setupAssignVarListNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector, unsigned i, std::string varName, Node* procNode, Node*& assignStm);
	void catchUnequalBracesException(std::stack<std::string>& bracesStack, unsigned bracesNo, unsigned i);
	void popImmediateParent(std::stack<std::string>& bracesStack, std::vector<Twin*>& twinVector, unsigned& bracesNo);
	void createCallNode(std::string line, int lineNumber, std::string& currentProcName, std::vector<Twin*>& twinVector, unsigned i);
	void createProcNode(std::string line, std::string& currentProcName, std::vector<Node*>& mainProg, std::stack<std::string>& bracesStack, std::vector<Twin*>& twinVector, unsigned i);
	void createAssignNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector, unsigned i);
	void createWhileNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector);
	void catchProcedureInceptionException(std::stack<std::string>&, unsigned, int);
	void catchDupProcedureException(unsigned i, Node* procStm);
	void catchRecursiveCallException(std::string& currentProcName, unsigned i, std::string& callProcName);
	void catchUnbalancedInfixException(unsigned i, bool isInflixBalance);
	std::vector<Node*> AST::buildAST(std::vector<std::string>);
};

#endif