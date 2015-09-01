#ifndef PKG_H
#define PKB_H

#include "stdafx.h"
#include "Node.h"
#include "Variable.h"
#include "VariableTable.h"
#include "UsesTable.h"
#include "ParentsTable.h"
#include "ModifiesTable.h"
#include "ExpressionTable.h"
#include "ProcedureTable.h"
#include "StatementTable.h"

class PKB {

	VariableTable variableTable;
	ModifiesTable modifiesTable;
	UsesTable usesTable;
	ParentsTable parentsTable;
	ExpressionTable expressionTable;
	ProcedureTable procedureTable;
	StatementTable statementTable;

public:
	PKB();

	Node* createProcedure(std::string);
	Node* createNode(synt_type);
	Node* createVariable(std::string);
	Node* createExpression(std::string);
	Node* createConstant(std::string);

	void setLeftChild(Node*, Node*);
	void setRightChild(Node*, Node*);
	void addStatement(Node*, Node*);

	std::vector<Variable> getModifies(std::string);
	std::vector<Variable> getModifies(int);
	std::vector<Variable> getModifies(synt_type);
	std::vector<Node*> getModifiedBy(std::string);
	std::vector<Node*> getModifiedBy(synt_type);

	std::vector<Variable> getUses(std::string);
	std::vector<Variable> getUses(synt_type);
	std::vector<Variable> getUses(int);
	std::vector<Node*> getUsers(std::string);
	std::vector<Variable> getUsers(synt_type);

	std::vector<Node*> getChildren(int);
	std::vector<Node*> getChildren(synt_type);
	Node* getParent(int);
	std::vector<Node*> getParents(synt_type);

	std::vector<Node*> getFollowing(int);
	std::vector<Node*> getFollowing(synt_type);
	Node* getFollower(int);
	std::vector<Node*> getFollowers(synt_type);

};

#endif