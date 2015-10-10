#ifndef PKB_H
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

using namespace std;

class PKB {

	VariableTable variableTable;
	ModifiesTable modifiesTable;
	UsesTable usesTable;
	ParentsTable parentsTable;
	ExpressionTable expressionTable;
	ProcedureTable procedureTable;
	StatementTable statementTable;
	vector<Node*> constants;

	int LIST_INDEX_OFFSET;
	void is_digit(string);

	void handleModifiedBy(Node*, Node*, Node*, Node*);
	void handleUsedBy(Node*, Node*, Node*, Node*);
	void handleParent(Node*, Node*);

	synt_type getStatementType(Node&);
	Node* getFollowing(Node*);
	Node* getFollowedBy(Node*);

public:
	PKB();

	Node* createProcedure(string);
	//type, statement/prog num, constant/procname/expr, UsedBy, ModifiedBy
	// Parent, ProcedureNode
	Node* createNode(synt_type, int, string="",
		Node* usedBy=nullptr, Node* modifiedBy=nullptr, Node* parent=nullptr, Node* procedure=nullptr); 

	void addStatement(Node*, Node*);

	vector<Node*> getNodes(synt_type);
	Variable* getVariable(string);
	int getCount(synt_type);

	vector<pair<string, string>> getModifies(synt_type, synt_type);
	vector<pair<string, string>> getModifies(synt_type, string);
	vector<pair<string, string>> getModifies(string, synt_type);
	vector<pair<string, string>> getModifies(string, string);

	vector<pair<string, string>> getUses(synt_type, synt_type);
	vector<pair<string, string>> getUses(synt_type, string);
	vector<pair<string, string>> getUses(string, synt_type);
	vector<pair<string, string>> getUses(string, string);

	vector<pair<string, string>> getParents(synt_type, synt_type);
	vector<pair<string, string>> getParents(synt_type, string);
	vector<pair<string, string>> getParents(string, synt_type);
	vector<pair<string, string>> getParents(string, string);

	//argument is the _ in parent ( _ , s )
	vector<Node*> getChildren(string);
	vector<Node*> getChildren(synt_type);

	//argument is the _ in parent ( s , _ )
	vector<Node*> getParent(string);
	vector<Node*> getParents(synt_type);

	//argument is the _ in follows ( _ , s )
	vector<Node*> getFollowing(string);
	vector<Node*> getFollowing(synt_type);

	//argument is the _ in follows ( s , _ )
	vector<Node*> getFollowedBy(string);
	vector<Node*> getFollowedBy(synt_type);
	
	vector<Node*> getExpressions(string);
	vector<Node*> getRootExpressions(string);
};

#endif