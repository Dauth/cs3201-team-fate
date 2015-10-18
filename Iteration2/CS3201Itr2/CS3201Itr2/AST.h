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


public:
	AST(PKB*, ExpressionTree*);
	void catchParamProcException(std::string line, unsigned i);
	void isCharABrace(bool& isBraces, char cChar);
	void catchMissingStmNameException(std::string& stmString, std::string& stmNameString, unsigned i);
	void catchIllegalOpenBraceException(unsigned i);
	bool isEqualSignPresent(std::string line);
	void getStatementTypeNew(std::string& stmString, int& statementType, char cChar, bool isSpace);
	void appendStmTokens(std::string& stmString, std::string& stmNameString, int statementType, bool isBraces, char cChar, bool isSpace, bool isSemiColon);
	void isCharAColon(bool& isSemiColon, char cChar);
	void clearStmHolder(std::string& stmString, std::string& stmNameString, int& statementType);
	bool getIndexOfThen(std::string& stmNameString, int & indexOfThen);
	void catchThenException(unsigned i, bool isThenPresent);
	void catchElseBeforeIfStmException(unsigned i, SyntType prevSynType);
	void catchEmptyContainerException(std::vector<Twin*>& twinVector, unsigned i);
	void setupWhileVarListNode(int lineNumber, std::vector<Twin*>& twinVector, std::string varName, Node*& whileStm, Node* procNode, Node*& whileStmLst);
	void setupIfThenListNode(int lineNumber, std::vector<Twin*>& twinVector, std::string varName, Node*& ifStm, Node* procNode, Node*& ifStmLst, Node*& thenStmLst);
	void extractExpressionVar(std::string& line, std::vector<std::string>& assignVector);
	void catchExpressionException(unsigned i, std::vector<std::string>& assignVector);
	void setupAssignVarListNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector, unsigned i, std::string varName, Node* procNode, Node*& assignStm);
	void catchUnequalBracesException(std::stack<std::string>& bracesStack, unsigned i);
	void popImmediateParent(std::stack<std::string>& bracesStack, std::vector<Twin*>& twinVector);
	void createCallNode(std::string stmName, int lineNumber, std::string& currentProcName, std::vector<Twin*>& twinVector, unsigned i);
	void createProcNode(std::string& currentProcName, std::vector<Node*>& mainProg, std::stack<std::string>& bracesStack, std::vector<Twin*>& twinVector, unsigned i);
	void createAssignNode(std::string line, int lineNumber, std::vector<Twin*>& twinVector, unsigned i);
	void createWhileNode(std::string varName, int lineNumber, std::vector<Twin*>& twinVector);
	void createIfElseNode(std::string varName, int lineNumber, std::vector<Twin*>& twinVector);
	void catchProcedureInceptionException(std::vector<Twin*>, unsigned, int);
	void catchDupProcedureException(unsigned i, Node* procStm);
	void catchRecursiveCallException(std::string& currentProcName, unsigned i, std::string& callProcName);
	void catchUnbalancedInfixException(unsigned i, bool isInflixBalance);
	std::vector<Node*> AST::buildAST(std::vector<std::string>);
	bool AST::isParamProcedure(std::string input);
};

#endif