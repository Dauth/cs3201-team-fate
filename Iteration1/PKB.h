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

	void handleModifiedBy(Node*, Node*, Node*);
	void handleUsedBy(Node*, Node*, Node*);
	void handleParent(Node*, Node*);

	synt_type getStatementType(Node&);
	Node* getFollowing(Node*);
	Node* getFollowedBy(Node*);

public:
	PKB();

	Node* createProcedure(std::string);
	//type, statement/prog num, constant/procname/expr, UsedBy, ModifiedBy
	// Parent, ProcedureNode
	Node* createNode(synt_type, int, std::string, Node*, Node*, Node*, Node*); 

	void addStatement(Node*, Node*);

	std::vector<Node*> getStatement(synt_type);
	int getCount(synt_type);

	//argument is the _ in modifies ( _ , v )
	std::vector<Node*> getModifies(std::string);   // proc name, returns nodes proc modifies
	std::vector<Node*> getModifies(int);           // statement num, return nodes modified by statement
	std::vector<Node*> getModifies(synt_type);     // statement type, returns all nodes modified by statement type

	//argument is the _ in modifies ( p/s , _ )
	std::vector<Node*> getModifiedBy(std::string); // variable name, returns all nodes that modify this var
	std::vector<Node*> getModifiedBy(synt_type);   // statement type (should always be variable), returns all statements that modify

	//argument is the _ in uses ( _ , v )
	std::vector<Node*> getUses(std::string);       // proc name, returns node proc uses
	std::vector<Node*> getUses(int);               // statement num, returns nodes used by statement
	std::vector<Node*> getUses(synt_type);         // statement type, returns all nodes used by statement type

	//argument is the _ in uses ( p/s, _ )
	std::vector<Node*> getUsedBy(std::string);      // variable name, returns all nodes that uses this var
	std::vector<Node*> getUsedBy(synt_type);        // statement type (should always be variable), returns all statements that use

	//argument is the _ in parent ( _ , s )
	std::vector<Node*> getChildren(int);
	std::vector<Node*> getChildren(synt_type);

	//argument is the _ in parent ( s , _ )
	std::vector<Node*> getParent(int);
	std::vector<Node*> getParents(synt_type);

	//argument is the _ in follows ( _ , s )
	std::vector<Node*> getFollowing(int);
	std::vector<Node*> getFollowing(synt_type);

	//argument is the _ in follows ( s , _ )
	std::vector<Node*> getFollowedBy(int);
	std::vector<Node*> getFollowedBy(synt_type);
	
	std::vector<Node*> getExpressions(std::string);
};

#endif