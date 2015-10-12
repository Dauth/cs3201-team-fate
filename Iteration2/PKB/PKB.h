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

	void handleModifiedBy(Node*, Node*, Node*, Node*);
	void handleUsedBy(Node*, Node*, Node*, Node*);
	void handleParent(Node*, Node*);

	SyntType getStatementType(Node&);
	Node* getFollowee(Node*);
	Node* getFollowedBy(Node*);

public:
	PKB(ExpressionTree*);

	Node* createProcedure(string);
	//type, statement/prog num, constant/procname/expr, UsedBy, ModifiedBy
	// Parent, ProcedureNode
	Node* createNode(SyntType, int, string="",
		Node* usedBy=nullptr, Node* modifiedBy=nullptr, Node* parent=nullptr, Node* procedure=nullptr); 

	void addStatement(Node*, Node*);

	vector<Node*> getNodes(SyntType);
	Variable* getVariable(string);
	int getCount(SyntType);

	vector<pair<string, string>> getModifies(SyntType, SyntType);
	vector<pair<string, string>> getModifies(SyntType, string);
	vector<pair<string, string>> getModifies(string, SyntType);
	vector<pair<string, string>> getModifies(string, string);

	vector<pair<string, string>> getUses(SyntType, SyntType);
	vector<pair<string, string>> getUses(SyntType, string);
	vector<pair<string, string>> getUses(string, SyntType);
	vector<pair<string, string>> getUses(string, string);

	vector<pair<string, string>> getParents(SyntType, SyntType);
	vector<pair<string, string>> getParents(SyntType, string);
	vector<pair<string, string>> getParents(string, SyntType);
	vector<pair<string, string>> getParents(string, string);

	vector<pair<string, string>> getParentsStar(SyntType, SyntType);
	vector<pair<string, string>> getParentsStar(SyntType, string);
	vector<pair<string, string>> getParentsStar(string, SyntType);
	vector<pair<string, string>> getParentsStar(string, string);

	vector<pair<string, string>> getFollows(SyntType, SyntType);
	vector<pair<string, string>> getFollows(SyntType, string);
	vector<pair<string, string>> getFollows(string, SyntType);
	vector<pair<string, string>> getFollows(string, string);

	vector<pair<string, string>> getFollowsStar(SyntType, SyntType);
	vector<pair<string, string>> getFollowsStar(SyntType, string);
	vector<pair<string, string>> getFollowsStar(string, SyntType);
	vector<pair<string, string>> getFollowsStar(string, string);

	vector<pair<string, string>> getCalls(SyntType, SyntType);
	vector<pair<string, string>> getCalls(SyntType, string);
	vector<pair<string, string>> getCalls(string, SyntType);
	vector<pair<string, string>> getCalls(string, string);

	vector<pair<string, string>> getCallsStar(SyntType, SyntType);
	vector<pair<string, string>> getCallsStar(SyntType, string);
	vector<pair<string, string>> getCallsStar(string, SyntType);
	vector<pair<string, string>> getCallsStar(string, string);
	
	vector<Node*> getExpressions(string);
	vector<Node*> getRootExpressions(string);
	vector<pair<string, string>> searchWithPattern(SyntType,std::string,std::string);
};

#endif