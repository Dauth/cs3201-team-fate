#include "stdafx.h"
#include "PKB.h"
#include <iostream>
#include <sstream>
#include "ExpressionTree.h";

using namespace std;

PKB::PKB(ExpressionTree* _et) {
	LIST_INDEX_OFFSET = 1;
	et = _et;
}

vector<Node*> PKB::getNodes(SyntType st) {
	if(st == variable) {
		return variableTable.getVariableNodes();
	} else if(st == constant) {
		return constants;
	}else if(st == procedure) {
		return procedureTable.getAllProcedures();
	} else if (st == progline) {
		statementTable.getStatements(statement);
	}
	return statementTable.getStatements(st);
}

Variable* PKB::getVariable(string varName) {
	return variableTable.getOrCreateVariable(varName);
}

int PKB::getCount(SyntType st) {
	if(st == variable) {
		return variableTable.getVariableCount();
	}
	return statementTable.getStatementCount(st);
}

// if returns nullpt procedure already exists
Node* PKB::createProcedure(string procName) {
	return procedureTable.createProcedure(procName);
}

Node* PKB::createNode(SyntType st, int line, string value, 
	Node* usedBy, Node* modifiedBy, Node* parent, Node* procedure) {
	ostringstream oss;
	oss << line;
	Node* node = new Node(st, oss.str(), value);
	if (st == variable) {
		Variable* var = variableTable.getOrCreateVariable(value);
		variableTable.addNode(value, node);
		node->setVar(var);
	} else if (st == constant) {
		constants.push_back(node);
	}
	if (st == whileLoop || st == ifelse || st == assignment || st == call) {
		statementTable.addStatement(oss.str(), node);
	} else if (st == expression ) {
		expressionTable.addExpression(node);
		node->setExpParent(usedBy);
	}
	if ((st == constant || st == variable ) && usedBy != nullptr && usedBy->getType() == assignment) {
		expressionTable.addExpression(node);
		node->setExpParent(usedBy);
	}
	if (modifiedBy != nullptr && st == variable) {
		handleModifiedBy(node, modifiedBy, procedure, parent);
	}
	if (usedBy != nullptr && st == variable) {
		handleUsedBy(node, usedBy, procedure, parent);
	}
	if (parent != nullptr && ((st == assignment) || (st == whileLoop) || (st == ifelse))) {
		handleParent(node, parent);
	}
	if (st == call) {
		callsTable.addCalls(procedure, node);
	}
	return node;
}

void PKB::handleModifiedBy(Node* node, Node* modifiedBy, Node* procedure, Node* parent) {
	modifiesTable.addModifies(modifiedBy, node);
	modifiesTable.addModifies(procedure, node);
	variableTable.addModifiedBy(node->getVariable()->getName(), modifiedBy);
	variableTable.addModifiedBy(node->getVariable()->getName(), procedure);
	while (parent != nullptr) {
		modifiesTable.addModifies(parent, node);
		variableTable.addModifiedBy(node->getVariable()->getName(), parent);
		Node* grandParent = parent->getParent()->getParent();
		if (grandParent->getType() == whileLoop || grandParent->getType() == ifelse) {
			parent = grandParent;
		} else {
			parent = nullptr;
		}
	}
}

void PKB::handleUsedBy(Node* node, Node* usedBy, Node* procedure, Node* parent) {
	usesTable.addUses(usedBy, node);
	usesTable.addUses(procedure, node);
	variableTable.addUsedBy(node->getVariable()->getName(), usedBy);
	variableTable.addUsedBy(node->getVariable()->getName(), procedure);
	while (parent != nullptr) {
		usesTable.addUses(parent, node);
		variableTable.addUsedBy(node->getVariable()->getName(), parent);
		Node* grandParent = parent->getParent()->getParent();
		if (grandParent->getType() == whileLoop || grandParent->getType() == ifelse) {
			parent = grandParent;
		} else {
			parent = nullptr;
		}
	}
}

void PKB::handleParent(Node* child, Node* parent) {
	parentsTable.addChild(parent, child);
}


vector<pair<string, string>> PKB::getModifies(SyntType st1, SyntType st2) {
	return modifiesTable.getByLeftKey(st1);
}

vector<pair<string, string>> PKB::getModifies(SyntType st, string varName) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = modifiesTable.getByLeftKey(st);
	vector<pair<string, string>> temp2 = modifiesTable.getByRightKey(varName);
	
	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for (int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(varName) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for (int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (st == procedure) {
				Node* procNode = procedureTable.getProcedure(left);
				if (procNode != nullptr) {
					results.push_back(temp2[i]);
				}
			} else {
				Node* stmtNode = statementTable.getStatement(left);
				if (stmtNode != nullptr && ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType())) {
					results.push_back(temp2[i]);
				}
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getModifies(string ident, SyntType st) {
	return modifiesTable.getByLeftKey(ident);
}

vector<pair<string, string>> PKB::getModifies(string ident, string varName) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = modifiesTable.getByLeftKey(ident);
	vector<pair<string, string>> temp2 = modifiesTable.getByRightKey(varName);

	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for (int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(varName) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for (int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (left.compare(ident) == 0) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getUses(SyntType st1, SyntType st2) {
	return usesTable.getByLeftKey(st1);
}

vector<pair<string, string>> PKB::getUses(SyntType st, string varName) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = usesTable.getByLeftKey(st);
	vector<pair<string, string>> temp2 = usesTable.getByRightKey(varName);
	
	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for (int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(varName) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for(int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (st == procedure) {
				Node* procNode = procedureTable.getProcedure(left);
				if (procNode != nullptr) {
					results.push_back(temp2[i]);
				}
			} else {
				Node* stmtNode = statementTable.getStatement(left);
				if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
					results.push_back(temp2[i]);
				}
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getUses(string ident, SyntType st) {
	return usesTable.getByLeftKey(ident);
}

vector<pair<string, string>> PKB::getUses(string ident, string varName) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = usesTable.getByLeftKey(ident);
	vector<pair<string, string>> temp2 = usesTable.getByRightKey(varName);

	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for (int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(varName) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for (int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (left.compare(ident) == 0) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParents(SyntType st1, SyntType st2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = parentsTable.getByLeftKey(st1);
	vector<pair<string, string>> temp2 = parentsTable.getByRightKey(st2);
	
	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for (int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			Node* stmtNode = statementTable.getStatement(right);
			if ((stmtNode->isStatement() && st2 == statement) || st2 == stmtNode->getType()) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for (int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			Node* stmtNode = statementTable.getStatement(left);
			if ((stmtNode->isStatement() && st1 == statement) || st1 == stmtNode->getType()) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParents(SyntType st, string stmtNum) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = parentsTable.getByLeftKey(st);
	vector<pair<string, string>> temp2 = parentsTable.getByRightKey(stmtNum);
	
	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for (int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(stmtNum) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for (int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			Node* stmtNode = statementTable.getStatement(left);
			if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParents(string stmtNum, SyntType st) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = parentsTable.getByLeftKey(stmtNum);
	vector<pair<string, string>> temp2 = parentsTable.getByRightKey(st);
	
	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for (int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			Node* stmtNode = statementTable.getStatement(right);
			if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for (int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (left.compare(stmtNum) == 0) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParents(string stmtNum1, string stmtNum2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = parentsTable.getByLeftKey(stmtNum1);
	vector<pair<string, string>> temp2 = parentsTable.getByRightKey(stmtNum2);

	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for (int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(stmtNum2) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for (int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (left.compare(stmtNum1) == 0) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParentsStar(SyntType st1, SyntType st2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> parents = getParents(st1, statement);
	if (parents.empty()){
		return parents;
	}

	for (int i=0; i<parents.size(); i++) {
		string left = parents[i].first;
		string right = parents[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		if (stmtNode->isContainer()) {
			vector<pair<string, string>> indirectParents = getParentsStar(stmtNode->getLine(), statement);
			for (int j=0; j<indirectParents.size(); j++) {
				string indirectRight = indirectParents[i].second;
				Node* indirectStmtNode = statementTable.getStatement(indirectRight);
				if ((indirectStmtNode->isStatement() && st2 == statement) || st2 == indirectStmtNode->getType()) {
					pair<string, string> parentsStar ( left, indirectRight );
					results.push_back(parentsStar);
				}
			}
		} 
		if ((stmtNode->isStatement() && st2 == statement) || st2 == stmtNode->getType()) {
			results.push_back(parents[i]);
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParentsStar(SyntType st, string stmtNum) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> parents = getParents(st, stmtNum);
	if (parents.empty()){
		return parents;
	}

	for (int i=0; i<parents.size(); i++) {
		string left = parents[i].first;
		string right = parents[i].second;
		Node* stmtNode = statementTable.getStatement(left);
		vector<pair<string, string>> indirectParents = getParentsStar(st, left);
		for (int j=0; j<indirectParents.size(); j++) {
			string indirectLeft = indirectParents[i].second;
			Node* indirectStmtNode = statementTable.getStatement(indirectLeft);
			if ((indirectStmtNode->isStatement() && st == statement) || st == indirectStmtNode->getType()) {
				pair<string, string> parentsStar ( indirectLeft, right );
				results.push_back(parentsStar);
			}
		}
		if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
			results.push_back(parents[i]);
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParentsStar(string stmtNum, SyntType st) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> parents = getParents(stmtNum, statement);
	if (parents.empty()){
		return parents;
	}

	for (int i=0; i<parents.size(); i++) {
		string left = parents[i].first;
		string right = parents[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		if (stmtNode->isContainer()) {
			vector<pair<string, string>> indirectParents = getParentsStar(stmtNode->getLine(), statement);
			for (int j=0; j<indirectParents.size(); j++) {
				string indirectRight = indirectParents[i].second;
				Node* indirectStmtNode = statementTable.getStatement(indirectRight);
				if ((indirectStmtNode->isStatement() && st == statement) || st == indirectStmtNode->getType()) {
					pair<string, string> parentsStar ( left, indirectRight );
					results.push_back(parentsStar);
				}
			}
		}
		if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
			results.push_back(parents[i]);
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParentsStar(string stmtNum1, string stmtNum2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> parents = getParents(stmtNum1, statement);
	if (parents.empty()){
		return parents;
	}

	for (int i=0; i<parents.size(); i++) {
		string left = parents[i].first;
		string right = parents[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		if (right.compare(stmtNum2) == 0) {
			results.push_back(parents[i]);
		} else if (stmtNode->isContainer()) {
			vector<pair<string, string>> indirectParents = getParentsStar(stmtNode->getLine(), statement);
			for (int j=0; j<indirectParents.size(); j++) {
				string indirectRight = indirectParents[i].second;
				if (indirectRight.compare(stmtNum2) == 0){
					pair<string, string> parentsStar ( left, indirectRight );
					results.push_back(parentsStar);
				}
			}
		}
	}
	return results;
}


vector<pair<string, string>> PKB::getFollows(SyntType st1, SyntType st2) {
	vector<pair<string, string>> results;
	vector<Node*> temp1 = statementTable.getStatements(st1);
	vector<Node*> temp2 = statementTable.getStatements(st2);

	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for(int i=0; i<temp1.size(); i++) {
			string left = temp1[i]->getLine();
			Node* stmtNode = statementTable.getStatement(left);
			Node* rightNode = getFollowee(stmtNode);
			if (rightNode != nullptr && (((rightNode->isStatement() && st2 == statement)) || st2 == rightNode->getType())) {
				pair<string, string> follows ( left, rightNode->getLine() );
				results.push_back(follows);
			}
		}
	} else {
		for(int i=0; i<temp2.size(); i++) {
			string right = temp2[i]->getLine();
			Node* stmtNode = statementTable.getStatement(right);
			Node* leftNode = getFollowedBy(stmtNode);
			if (leftNode != nullptr && (((leftNode->isStatement() && st1 == statement)) || st1 == leftNode->getType())) {
				pair<string, string> follows ( leftNode->getLine(), right );
				results.push_back(follows);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getFollows(SyntType st, string stmtNum) {
	vector<pair<string, string>> results;
	Node* rightNode = statementTable.getStatement(stmtNum);
	if (rightNode != nullptr) {
		Node* leftNode = getFollowedBy(rightNode);
		if (leftNode != nullptr && (((leftNode->isStatement() && st == statement)) || st == leftNode->getType())) {
			pair<string, string> follows ( leftNode->getLine(), rightNode->getLine() );
			results.push_back(follows);
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getFollows(string stmtNum, SyntType st) {
	vector<pair<string, string>> results;
	Node* leftNode = statementTable.getStatement(stmtNum);
	if (leftNode != nullptr) {
		Node* rightNode = getFollowee(leftNode);
		if (rightNode != nullptr && (((rightNode->isStatement() && st == statement)) || st == rightNode->getType())) {
			pair<string, string> follows ( leftNode->getLine(), rightNode->getLine() );
			results.push_back(follows);
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getFollows(string stmtNum1, string stmtNum2) {
	vector<pair<string, string>> results;
	Node* leftNode = statementTable.getStatement(stmtNum1);
	if (leftNode != nullptr) {
		Node* rightNode = getFollowee(leftNode);
		if (rightNode->getLine().compare(stmtNum2) == 0) {
			pair<string, string> follows ( leftNode->getLine(), rightNode->getLine() );
			results.push_back(follows);
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getFollowsStar(SyntType st1, SyntType st2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> follows = getFollows(st1, statement);
	if (follows.empty()){
		return follows;
	}

	for (int i=0; i<follows.size(); i++) {
		string left = follows[i].first;
		string right = follows[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		vector<pair<string, string>> indirectFollows = getFollowsStar(stmtNode->getLine(), statement);
		for (int j=0; j<indirectFollows.size(); j++) {
			string indirectRight = indirectFollows[i].second;
			Node* indirectStmtNode = statementTable.getStatement(indirectRight);
			if ((indirectStmtNode->isStatement() && st2 == statement) || st2 == indirectStmtNode->getType()) {
				pair<string, string> followsStar ( left, indirectRight );
				results.push_back(followsStar);
			}
		}
		if ((stmtNode->isStatement() && st2 == statement) || st2 == stmtNode->getType()) {
			results.push_back(follows[i]);
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getFollowsStar(SyntType st, string stmtNum) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> follows = getFollows(st, stmtNum);
	if (follows.empty()){
		return follows;
	}

	for (int i=0; i<follows.size(); i++) {
		string left = follows[i].first;
		string right = follows[i].second;
		Node* stmtNode = statementTable.getStatement(left);
		vector<pair<string, string>> indirectFollows = getFollowsStar(st, left);
		for (int j=0; j<indirectFollows.size(); j++) {
			string indirectLeft = indirectFollows[i].second;
			Node* indirectStmtNode = statementTable.getStatement(indirectLeft);
			if ((indirectStmtNode->isStatement() && st == statement) || st == indirectStmtNode->getType()) {
				pair<string, string> followsStar ( indirectLeft, right );
				results.push_back(followsStar);
			}
		}
		if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
			results.push_back(follows[i]);
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getFollowsStar(string stmtNum, SyntType st) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> follows = getFollows(stmtNum, statement);
	if (follows.empty()){
		return follows;
	}

	for (int i=0; i<follows.size(); i++) {
		string left = follows[i].first;
		string right = follows[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		vector<pair<string, string>> indirectFollows = getFollowsStar(stmtNode->getLine(), statement);
		for (int j=0; j<indirectFollows.size(); j++) {
			string indirectRight = indirectFollows[i].second;
			Node* indirectStmtNode = statementTable.getStatement(indirectRight);
			if ((indirectStmtNode->isStatement() && st == statement) || st == indirectStmtNode->getType()) {
				pair<string, string> followsStar ( left, indirectRight );
				results.push_back(indirectFollows[i]);
			}
		}
		if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
			results.push_back(follows[i]);
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getFollowsStar(string stmtNum1, string stmtNum2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> follows = getFollows(stmtNum1, statement);
	if (follows.empty()){
		return follows;
	}

	for (int i=0; i<follows.size(); i++) {
		string left = follows[i].first;
		string right = follows[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		if (right.compare(stmtNum2) == 0) {
			results.push_back(follows[i]);
		} else {
			vector<pair<string, string>> indirectFollows = getFollowsStar(stmtNode->getLine(), statement);
			for (int j=0; j<indirectFollows.size(); j++) {
				string indirectRight = indirectFollows[i].second;
				if (indirectRight.compare(stmtNum2) == 0){
					pair<string, string> followsStar ( left, indirectRight );
					results.push_back(followsStar);
				}
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getCalls(SyntType st1, SyntType st2) {
	return callsTable.getAll();
}

vector<pair<string, string>> PKB::getCalls(SyntType st, string stmtNum) {
	return callsTable.getByRightKey(stmtNum);
}

vector<pair<string, string>> PKB::getCalls(string stmtNum, SyntType st) {
	return callsTable.getByLeftKey(stmtNum);
}

vector<pair<string, string>> PKB::getCalls(string stmtNum1, string stmtNum2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = callsTable.getByLeftKey(stmtNum1);
	vector<pair<string, string>> temp2 = callsTable.getByRightKey(stmtNum2);

	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for(int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(stmtNum2) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for(int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (left.compare(stmtNum2) == 0) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getCallsStar(SyntType st1, SyntType st2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(st1, st2);
	if (calls.empty()){
		return calls;
	}

	for (int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;
		vector<pair<string, string>> indirectCalls = getCallsStar(right, st2);
		for (int j=0; j<indirectCalls.size(); j++) {
			string indirectRight = indirectCalls[i].second;
			pair<string, string> callsStar ( left, indirectRight );
			results.push_back(callsStar);
		}
		results.push_back(calls[i]);
	}
	return results;
}

vector<pair<string, string>> PKB::getCallsStar(SyntType st, string procName) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(st, procName);
	if (calls.empty()){
		return calls;
	}

	for (int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;
		vector<pair<string, string>> indirectCalls = getCallsStar(st, left);
		for (int j=0; j<indirectCalls.size(); j++) {
			string indirectLeft = indirectCalls[i].first;
			pair<string, string> callsStar ( indirectLeft, right );
			results.push_back(callsStar);
		}
		results.push_back(calls[i]);
	}
	return results;
}

vector<pair<string, string>> PKB::getCallsStar(string procName, SyntType st) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(procName, st);
	if (calls.empty()){
		return calls;
	}

	for (int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;
		vector<pair<string, string>> indirectCalls = getCallsStar(right, st);
		for (int j=0; j<indirectCalls.size(); j++) {
			string indirectRight = indirectCalls[i].second;
			pair<string, string> callsStar ( left, indirectRight );
			results.push_back(callsStar);
		}
		results.push_back(calls[i]);
	}
	return results;
}

vector<pair<string, string>> PKB::getCallsStar(string procName1, string procName2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> calls = getFollows(procName1, procedure);
	if (calls.empty()){
		return calls;
	}

	for (int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;;
		if (right.compare(procName2) == 0) {
			results.push_back(calls[i]);
		} else {
			vector<pair<string, string>> indirectCalls = getFollowsStar(right, procedure);
			for (int j=0; j<indirectCalls.size(); j++) {
				string indirectRight = indirectCalls[i].second;
				if (indirectRight.compare(procName2) == 0){
					pair<string, string> followsStar ( left, indirectRight );
					results.push_back(followsStar);
				}
			}
		}
	}
	return results;
}

//private
Node* PKB::getFollowee(Node* statement) {
	Node* stmtLstNode = statement->getParent();
	int index = stmtLstNode->getIndexLst()[statement];
	if(index < (stmtLstNode->getIndexLst().size())) {
		return stmtLstNode->getStmtLst()[index + LIST_INDEX_OFFSET];
	}
	return nullptr;
}

//private
Node* PKB::getFollowedBy(Node* statement) {
	Node* stmtLstNode = statement->getParent();
	int index = stmtLstNode->getIndexLst()[statement];
	if(index > 0) {
		return stmtLstNode->getStmtLst()[index - LIST_INDEX_OFFSET];
	}
	return nullptr;
}

vector<Node*> PKB::getExpressions(string expr) {
	return expressionTable.getExpressions(expr);
}

vector<Node*> PKB::getRootExpressions(string expr) {
	return expressionTable.getRootExpressions(expr);
}

vector<pair<string, string>> PKB::searchWithPattern(SyntType type,string left,string right){
	vector<pair<string, string>> results;
	string firstChar = right.substr(0,1);
	//if the pattern is for a while loop
	if (type == whileLoop){
		vector<Node*> whileList = getNodes(whileLoop);
		if (left == "_"){
			for (int i=0; i<whileList.size(); i++) {
				pair<string, string> stmtPair ( whileList[i]->getLine(), whileList[i]->getLeftChild()->getValue() );
				results.push_back(stmtPair);
			}
		}
		else{
			for (unsigned int i = 0 ; i < whileList.size() ; i++){
				if (whileList.at(i)->getLeftChild()->getVariable()->getName().compare(left) == 0){
					pair<string, string> stmtPair ( whileList[i]->getLine(), whileList[i]->getLeftChild()->getValue() );
					results.push_back(stmtPair);
				}
			}
		}
	} else if (type == ifelse){
		vector<Node*> ifList = getNodes(ifelse);
		if (left == "_"){
			for (int i=0; i<ifList.size(); i++) {
				pair<string, string> stmtPair ( ifList[i]->getLine(), ifList[i]->getLeftChild()->getValue() );
				results.push_back(stmtPair);
			}
		}
		else{
			for (unsigned int i = 0 ; i < ifList.size() ; i++){
				if (ifList.at(i)->getLeftChild()->getVariable()->getName().compare(left) == 0){
					pair<string, string> stmtPair ( ifList[i]->getLine(), ifList[i]->getLeftChild()->getValue() );
					results.push_back(stmtPair);
				}
			}
		}

	//if the pattern is for an assignment
	} else{
		//if the right side was "_"
		if (right == "_"){
			if (left == "_"){
				vector<Node*> nodes = getNodes(assignment);
				for (int i=0; i<nodes.size(); i++) {
					pair<string, string> stmtPair ( nodes[i]->getLine(), nodes[i]->getLeftChild()->getValue() );
					results.push_back(stmtPair);
				}
			}
			else{
				Variable* var = getVariable(left);
				vector<Node*> assignList  = var->getModifiedBy();
				for(unsigned int i = 0 ; i < assignList.size() ; i++){
					if(assignList.at(i)->getType() == assignment){
						pair<string, string> stmtPair ( assignList[i]->getLine(), assignList[i]->getLeftChild()->getValue() );
						results.push_back(stmtPair);
					}
				}

				return results;
			}
		}
		//if the right side is not an expression (eg. variable or constant)
		if (!isExpression(right)){
			vector<Node*> exprList;
			if (firstChar == "_" ){
				exprList = getExpressions(right.substr(1,right.length()-2));
				for (unsigned int i = 0 ; i < exprList.size() ; i++ ){
					Node* parentNode = exprList.at(i)->getExpParent();
					if (left == "_"){
						pair<string, string> stmtPair ( parentNode->getLine(), parentNode->getLeftChild()->getValue() );
						results.push_back(stmtPair);
					}
					else{
						if(parentNode->getLeftChild()->getVariable()->getName() == left){
							pair<string, string> stmtPair ( parentNode->getLine(), parentNode->getLeftChild()->getValue() );
							results.push_back(stmtPair);
						}
					}
				}

				return results;
			}
			
			exprList = getRootExpressions(right);
			for (unsigned int i = 0 ; i < exprList.size() ; i++ ){
				Node* parentNode = exprList.at(i)->getParent();
				if ( left == "_" ){
					pair<string, string> stmtPair ( parentNode->getLine(), parentNode->getLeftChild()->getValue() );
					results.push_back(stmtPair);
				}
				else{
					if(parentNode->getLeftChild()->getVariable()->getName() == left){
						pair<string, string> stmtPair ( parentNode->getLine(), parentNode->getLeftChild()->getValue() );
						results.push_back(stmtPair);
					}
				}
			}
			return results;
		}

		//if the right was not "_" and it is an expression
		Node* tNode;
		vector<Node*> exprList;
		if (firstChar != "_"){
			tNode = et->exptreeSetupSON(et->expressionConverter(right));
			exprList = getRootExpressions(tNode->getValue());
		}

		else{
			tNode = et->exptreeSetupSON(et->expressionConverter(right.substr(1,right.length()-2)));
			exprList = getExpressions(tNode->getValue());
		}

		for (unsigned int i = 0 ; i < exprList.size() ; i++){
			Node* tNode2 = exprList.at(i);
			int compareResult = compare(tNode2,tNode);
			if(compareResult != 0){
				Node* parentNode = tNode2->getParent();
				if (firstChar != "_"){
					if(parentNode->getType() == assignment){
						if (left == "_"){
							pair<string, string> stmtPair ( parentNode->getLine(), parentNode->getLeftChild()->getValue() );
							results.push_back(stmtPair);
						}
						else{
							if(parentNode->getLeftChild()->getVariable()->getName() == left){
								pair<string, string> stmtPair ( parentNode->getLine(), parentNode->getLeftChild()->getValue() );
								results.push_back(stmtPair);
							}
						}
					}
				}
				else{
					parentNode = tNode2->getExpParent();
					if (left == "_"){
						pair<string, string> stmtPair ( parentNode->getLine(), parentNode->getLeftChild()->getValue() );
						results.push_back(stmtPair);
					}
					else{
						if(parentNode->getLeftChild()->getVariable()->getName() == left){
							pair<string, string> stmtPair ( parentNode->getLine(), parentNode->getLeftChild()->getValue() );
							results.push_back(stmtPair);
						}
					}
				}
			}
		}
	}

	return results;
}

int PKB::compare(Node* p,Node* q){
   if(p == NULL && q == NULL){
	   return 1;
   }
   else if(p == NULL || q == NULL){
	   return 0;
   }
   else if(p->getValue() == q->getValue()){
	   int s1=compare(p->getLeftChild(),q->getLeftChild());
	   int s2=compare(p->getRightChild(),q->getRightChild());
	   return s1*s2;
   }
   else
		return 0;
}
bool PKB::isExpression(string s){
	if(s.find("+") != string::npos || s.find("*") != string::npos || s.find("-") != string::npos){
		return true;
	}

	return false;
}