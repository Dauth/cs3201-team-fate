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

vector<Node*> PKB::getChildren(string statementLine) {
	Node* parent = statementTable.getStatement(statementLine);
	return parentsTable.getChild(parent);
}

vector<Node*> PKB::getChildren(synt_type st) {
	vector<Node*> parents = statementTable.getStatements(st);
	set<Node*> children;
	for(int i=0; i<parents.size(); i++) {
		vector<Node*> temp = parentsTable.getChild(parents[i]);
		for(int j=0; j<temp.size(); j++) {
			children.insert(temp[j]);
		}
	}
	return vector<Node*> (children.begin(), children.end());
}

vector<Node*> PKB::getParent(string statementLine) {
	vector<Node*> parents;
	Node* child = statementTable.getStatement(statementLine);
	if (child != nullptr) {
		Node* parent = child->getParent()->getParent();
		if(parent != nullptr && (parent->getType() == whileLoop || parent->getType() == ifelse)) {
			parents.push_back(parent);
		}
	}
	return parents;
}

vector<Node*> PKB::getParents(synt_type st) {
	vector<Node*> children = parentsTable.getChildren(st);
	set<Node*> parents;
	for(vector<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
		parents.insert((*it)->getParent()->getParent());
	}
	return vector<Node*> (parents.begin(), parents.end());
}

vector<Node*> PKB::getFollowing(string statementLine) {
	Node* statement = statementTable.getStatement(statementLine);
	vector<Node*> following;
	if (statement != nullptr) {
		Node* node = getFollowing(statement);
		if(node != nullptr) {
			following.push_back(node);
		}
	}
	return following;
}

//private
Node* PKB::getFollowing(Node* statement) {
	Node* stmtLstNode = statement->getParent();
	int index = stmtLstNode->getIndexLst()[statement];
	if(index < (stmtLstNode->getIndexLst().size())) {
		return stmtLstNode->getStmtLst()[index + LIST_INDEX_OFFSET];
	}
	return nullptr;
}

vector<Node*> PKB::getFollowing(synt_type st) {
	vector<Node*> statements = statementTable.getStatements(st);
	vector<Node*> following;
	for(vector<Node*>::iterator it = statements.begin(); it != statements.end(); ++it) {
		Node* followee = getFollowing(*it);
		if (followee != nullptr) {
			following.push_back(followee);
		}
	}
	return following;
}

vector<Node*> PKB::getFollowedBy(string statementLine) {
	Node* statement = statementTable.getStatement(statementLine);
	vector<Node*> followedBy;
	if (statement != nullptr) {
		Node* node = getFollowedBy(statement);
		if(node != nullptr) {
			followedBy.push_back(node);
		}
	}
	return followedBy;
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

vector<Node*> PKB::getFollowedBy(synt_type st) {
	vector<Node*> statements = statementTable.getStatements(st);
	vector<Node*> followers;
	for(vector<Node*>::iterator it = statements.begin(); it != statements.end(); ++it) {
		Node* follower = getFollowedBy(*it);
		if (follower != nullptr) {
			followers.push_back(follower);
		}
	}
	return followers;
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