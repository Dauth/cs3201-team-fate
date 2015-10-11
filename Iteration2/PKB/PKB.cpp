#include "stdafx.h"
#include "PKB.h"
#include <iostream>
#include <sstream>

using namespace std;

PKB::PKB () {
	LIST_INDEX_OFFSET = 1;
}

vector<Node*> PKB::getNodes(synt_type st) {
	if(st == variable) {
		return variableTable.getVariableNodes();
	} else if(st == constant) {
		return constants;
	}else if(st == procedure) {
		return procedureTable.getAllProcedures();
	}
	return statementTable.getStatements(st);
}

Variable* PKB::getVariable(string varName) {
	return variableTable.getOrCreateVariable(varName);
}

int PKB::getCount(synt_type st) {
	if(st == variable) {
		return variableTable.getVariableCount();
	}
	return statementTable.getStatementCount(st);
}

// if returns nullpt procedure already exists
Node* PKB::createProcedure(string procName) {
	return procedureTable.createProcedure(procName);
}

Node* PKB::createNode(synt_type st, int line, string value, 
	Node* usedBy, Node* modifiedBy, Node* parent, Node* procedure) {
	ostringstream oss;
	oss << line;
	Node* node = new Node(st, oss.str(), value);
	if(st == variable) {
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
		if(grandParent->getType() == whileLoop || grandParent->getType() == ifelse) {
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
		if(grandParent->getType() == whileLoop || grandParent->getType() == ifelse) {
			parent = grandParent;
		} else {
			parent = nullptr;
		}
	}
}

void PKB::handleParent(Node* child, Node* parent) {
	parentsTable.addChild(parent, child);
}


vector<pair<string, string>> PKB::getModifies(synt_type st1, synt_type st2) {
	return modifiesTable.getByLeftKey(st1);
}

vector<pair<string, string>> PKB::getModifies(synt_type st, string varName) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = modifiesTable.getByLeftKey(st);
	vector<pair<string, string>> temp2 = modifiesTable.getByRightKey(varName);
	
	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for(int i=0; i<temp1.size(); i++) {
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
				if (stmtNode != nullptr && ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType())) {
					results.push_back(temp2[i]);
				}
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getModifies(string ident, synt_type st) {
	return modifiesTable.getByLeftKey(ident);
}

vector<pair<string, string>> PKB::getModifies(string ident, string varName) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = modifiesTable.getByLeftKey(ident);
	vector<pair<string, string>> temp2 = modifiesTable.getByRightKey(varName);

	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for(int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(varName) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for(int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (left.compare(ident) == 0) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getUses(synt_type st1, synt_type st2) {
	return usesTable.getByLeftKey(st1);
}

vector<pair<string, string>> PKB::getUses(synt_type st, string varName) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = usesTable.getByLeftKey(st);
	vector<pair<string, string>> temp2 = usesTable.getByRightKey(varName);
	
	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for(int i=0; i<temp1.size(); i++) {
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

vector<pair<string, string>> PKB::getUses(string ident, synt_type st) {
	return usesTable.getByLeftKey(ident);
}

vector<pair<string, string>> PKB::getUses(string ident, string varName) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = usesTable.getByLeftKey(ident);
	vector<pair<string, string>> temp2 = usesTable.getByRightKey(varName);

	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for(int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(varName) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for(int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (left.compare(ident) == 0) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParents(synt_type st1, synt_type st2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = parentsTable.getByLeftKey(st1);
	vector<pair<string, string>> temp2 = parentsTable.getByRightKey(st2);
	
	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for(int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			Node* stmtNode = statementTable.getStatement(right);
			if ((stmtNode->isStatement() && st2 == statement) || st2 == stmtNode->getType()) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for(int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			Node* stmtNode = statementTable.getStatement(left);
			if ((stmtNode->isStatement() && st1 == statement) || st1 == stmtNode->getType()) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParents(synt_type st, string stmtNum) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = parentsTable.getByLeftKey(st);
	vector<pair<string, string>> temp2 = parentsTable.getByRightKey(stmtNum);
	
	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for(int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(stmtNum) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for(int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			Node* stmtNode = statementTable.getStatement(left);
			if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getParents(string stmtNum, synt_type st) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> temp1 = parentsTable.getByLeftKey(stmtNum);
	vector<pair<string, string>> temp2 = parentsTable.getByRightKey(st);
	
	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for(int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			Node* stmtNode = statementTable.getStatement(right);
			if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for(int i=0; i<temp2.size(); i++) {
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
		for(int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(stmtNum2) == 0) {
				results.push_back(temp1[i]);
			}
		}
	} else {
		for(int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (left.compare(stmtNum1) == 0) {
				results.push_back(temp2[i]);
			}
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getFollows(synt_type st1, synt_type st2) {
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

vector<pair<string, string>> PKB::getFollows(synt_type st, string stmtNum) {
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

vector<pair<string, string>> PKB::getFollows(string stmtNum, synt_type st) {
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

bool is_digit(string &s)
{
    string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) {
		++it;
	}
    return !s.empty() && it == s.end();
}