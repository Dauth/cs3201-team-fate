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
#include "CallsTable.h"
#include "ExpressionTree.h"

using namespace std;

class ExpressionTree;

class PKB {

	ExpressionTree *et;
	VariableTable variableTable;
	ModifiesTable modifiesTable;
	UsesTable usesTable;
	ParentsTable parentsTable;
	ExpressionTable expressionTable;
	ProcedureTable procedureTable;
	StatementTable statementTable;
	CallsTable callsTable;
	vector<Node*> constants;

	int LIST_INDEX_OFFSET;
	void is_digit(string);
	int compare(Node*, Node*);
	bool isExpression(std::string);

	void handleInheritance(Node*, Node*, Node*, Node*);
	void handleCalls(Node*, Node*);
	void handleModifiedBy(Node*, Node*, Node*, Node*);
	void handleUsedBy(Node*, Node*, Node*, Node*);
	void handleParent(Node*, Node*);

	synt_type getStatementType(Node&);
	Node* getFollowee(Node*);
	Node* getFollowedBy(Node*);

public:
	PKB(ExpressionTree*);

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

	vector<pair<string, string>> getParentsStar(synt_type, synt_type);
	vector<pair<string, string>> getParentsStar(synt_type, string);
	vector<pair<string, string>> getParentsStar(string, synt_type);
	vector<pair<string, string>> getParentsStar(string, string);

	vector<pair<string, string>> getFollows(synt_type, synt_type);
	vector<pair<string, string>> getFollows(synt_type, string);
	vector<pair<string, string>> getFollows(string, synt_type);
	vector<pair<string, string>> getFollows(string, string);

	vector<pair<string, string>> getFollowsStar(synt_type, synt_type);
	vector<pair<string, string>> getFollowsStar(synt_type, string);
	vector<pair<string, string>> getFollowsStar(string, synt_type);
	vector<pair<string, string>> getFollowsStar(string, string);

	vector<pair<string, string>> getCalls(synt_type, synt_type);
	vector<pair<string, string>> getCalls(synt_type, string);
	vector<pair<string, string>> getCalls(string, synt_type);
	vector<pair<string, string>> getCalls(string, string);

	vector<pair<string, string>> getCallsStar(synt_type, synt_type);
	vector<pair<string, string>> getCallsStar(synt_type, string);
	vector<pair<string, string>> getCallsStar(string, synt_type);
	vector<pair<string, string>> getCallsStar(string, string);
	
	vector<Node*> getExpressions(string);
	vector<Node*> getRootExpressions(string);
	vector<pair<string, string>> searchWithPattern(synt_type,std::string,std::string);
};

#endif