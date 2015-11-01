#ifndef PKB_H
#define PKB_H

#include "stdafx.h"
#include "Node.h"
#include "VariableTable.h"
#include "UsesTable.h"
#include "ParentsTable.h"
#include "ModifiesTable.h"
#include "ExpressionTable.h"
#include "ProcedureTable.h"
#include "StatementTable.h"
#include "CallsTable.h"
#include "ExpressionTree.h"
#include "CFG.h"

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
	unordered_map<string, vector<Node*>> constants;
	CFG cfg;

	int LIST_INDEX_OFFSET;
	void is_digit(string);
	int compare(Node*, Node*);
	bool isExpression(std::string);
	bool isUsed(string, string);
	bool isModified(string, string);

	 
	void handleInheritance(Node*, Node*, Node*, Node*);
	void handleCalls(Node*, Node*, Node*);
	void handleModifiedBy(Node*, Node*, Node*, Node*);
	void handleUsedBy(Node*, Node*, Node*, Node*);
	void handleParent(Node*, Node*);

	SyntType getStatementType(Node&);
	Node* getFollowee(Node*);
	Node* getFollowedBy(Node*);
	void forwardAffectsSearch(set<pair<string, string>>*, set<pair<string, Node*>>*, Node*, string, string, string, bool);
	void reverseAffectsSearch(set<pair<string, string>>*, set<pair<string, Node*>>*, Node*, string, string, bool);
public:
	PKB(ExpressionTree*);


	Node* createProcedure(string);
	//type, statement/prog num, constant/procname/expr, UsedBy, ModifiedBy
	// Parent, ProcedureNode
	Node* createNode(SyntType, int, string="",
		Node* usedBy=nullptr, Node* modifiedBy=nullptr, Node* parent=nullptr, Node* procedure=nullptr); 

	void buildCFG();

	void addStatement(Node*, Node*);

	vector<Node*> getNodes(SyntType);
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

	vector<pair<string, string>> getNext(SyntType, SyntType);
	vector<pair<string, string>> getNext(SyntType, string);
	vector<pair<string, string>> getNext(string, SyntType);
	vector<pair<string, string>> getNext(string, string);

	vector<pair<string, string>> getNextStar(SyntType, SyntType);
	vector<pair<string, string>> getNextStar(SyntType, string);
	vector<pair<string, string>> getNextStar(string, SyntType);
	vector<pair<string, string>> getNextStar(string, string);
	
	vector<pair<string, string>> getAffects(SyntType, SyntType);
	vector<pair<string, string>> getAffects(SyntType, string);
	vector<pair<string, string>> getAffects(string, SyntType);
	vector<pair<string, string>> getAffects(string, string);

	vector<pair<string, string>> getAffectsStar(SyntType, SyntType);
	vector<pair<string, string>> getAffectsStar(SyntType, string);
	vector<pair<string, string>> getAffectsStar(string, SyntType);
	vector<pair<string, string>> getAffectsStar(string, string);

	vector<Node*> getExpressions(string);
	vector<Node*> getRootExpressions(string);
	vector<pair<string, string>> searchWithPattern(SyntType,std::string,std::string);
};

#endif