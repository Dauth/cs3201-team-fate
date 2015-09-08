#include "stdafx.h"
#include "PKB.h"
#include <iostream>
PKB::PKB () {

}

std::vector<Node*> PKB::getNodes(synt_type st) {
	if(st == variable) {
		return variableTable.getVariableNodes();
	}
	if(st == constant) {
		return constants;
	}
	return statementTable.getStatements(st);
}

int PKB::getCount(synt_type st) {
	if(st == variable) {
		return variableTable.getVariableCount();
	}
	return statementTable.getStatementCount(st);
}

// if returns nullpt procedure already exists
Node* PKB::createProcedure(std::string procName) {
	return procedureTable.createProcedure(procName);
}

Node* PKB::createNode(synt_type st, int line, std::string value, 
	Node* usedBy, Node* modifiedBy, Node* parent, Node* procedure) {
	Node* node = new Node(st, line, value);
	if(st == variable) {
		Variable* var = variableTable.getOrCreateVariable(value);
		variableTable.addNode(value, node);
		node->setVar(var);
	} else if (st == constant) {
		constants.push_back(node);
	}
	if (st == whileLoop || st == ifelse || st == assignment || st == call) {
		statementTable.addStatement(line, node);
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
	usesTable.addUsedBy(usedBy, node);
	usesTable.addUsedBy(procedure, node);
	variableTable.addUsedBy(node->getVariable()->getName(), usedBy);
	variableTable.addUsedBy(node->getVariable()->getName(), procedure);
	while (parent != nullptr) {
		usesTable.addUsedBy(parent, node);
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

std::vector<Node*> PKB::getModifies(std::string procName) {
	Node* procedure = procedureTable.getProcedure(procName);
	return modifiesTable.getModified(procedure);
}

std::vector<Node*> PKB::getModifies(int statementLine) {
	Node* node = statementTable.getStatement(statementLine);
	return modifiesTable.getModified(node);
}

std::vector<Node*> PKB::getModifies(synt_type st) {
	std::vector<Node*> modifiers = modifiesTable.getModifiers(st);
	std::set<Node*> modifies;
	for(int i=0; i<modifiers.size(); i++) {
		std::vector<Node*> temp = modifiesTable.getModified(modifiers[i]);
		for(int j=0; j<temp.size(); j++) {
			modifies.insert(temp[j]);
		}
	}
	return std::vector<Node*> (modifies.begin(), modifies.end());
}

std::vector<Node*> PKB::getModifiedBy(std::string varName) {
	Variable* variable = variableTable.getOrCreateVariable(varName);
	return variable->getModifiedBy();
}

std::vector<Node*> PKB::getModifiedBy(synt_type st) {
	std::vector<Node*> modifiers;
	if(st == variable) {
		modifiers = modifiesTable.getModifiers();
	}
	return modifiers;
}

std::vector<Node*> PKB::getUses(std::string procName) {
	Node* procedure = procedureTable.getProcedure(procName);
	return usesTable.getUsed(procedure);
}

std::vector<Node*> PKB::getUses(int statementLine) {
	Node* node = statementTable.getStatement(statementLine);
	return usesTable.getUsed(node);
}

std::vector<Node*> PKB::getUses(synt_type st) {
	std::vector<Node*> users = usesTable.getUsers(st);
	std::set<Node*> uses;
	for(int i=0; i<users.size(); i++) {
		std::vector<Node*> temp = usesTable.getUsed(users[i]);
		for(int j=0; j<temp.size(); j++) {
			uses.insert(temp[j]);
		}
	}
	return std::vector<Node*> (uses.begin(), uses.end());
}

std::vector<Node*> PKB::getUsedBy(std::string varName) {
	return variableTable.getUsedBy(varName);
}

std::vector<Node*> PKB::getUsedBy(synt_type st) {
	std::vector<Node*> users;
	if(st == variable) {
		users = usesTable.getUsers();
	}
	return users;
}

std::vector<Node*> PKB::getChildren(int statementLine) {
	Node* parent = statementTable.getStatement(statementLine);
	return parentsTable.getChild(parent);
}

std::vector<Node*> PKB::getChildren(synt_type st) {
	std::vector<Node*> parents = statementTable.getStatements(st);
	std::set<Node*> children;
	for(int i=0; i<parents.size(); i++) {
		std::vector<Node*> temp = parentsTable.getChild(parents[i]);
		for(int j=0; j<temp.size(); j++) {
			children.insert(temp[j]);
		}
	}
	return std::vector<Node*> (children.begin(), children.end());
}

std::vector<Node*> PKB::getParent(int statementLine) {
	std::vector<Node*> parents;
	Node* child = statementTable.getStatement(statementLine);
	if (child != nullptr) {
		Node* parent = child->getParent()->getParent();
		if(parent != nullptr && (parent->getType() == whileLoop || parent->getType() == ifelse)) {
			parents.push_back(parent);
		}
	}
	return parents;
}

std::vector<Node*> PKB::getParents(synt_type st) {
	std::vector<Node*> children = parentsTable.getChildren(st);
	std::set<Node*> parents;
	for(std::vector<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
		parents.insert((*it)->getParent()->getParent());
	}
	return std::vector<Node*> (parents.begin(), parents.end());
}

std::vector<Node*> PKB::getFollowing(int statementLine) {
	Node* statement = statementTable.getStatement(statementLine);
	std::vector<Node*> following;
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
		return stmtLstNode->getStmtLst()[index + 1]; // magic number!!!
	}
	return nullptr;
}

std::vector<Node*> PKB::getFollowing(synt_type st) {
	std::vector<Node*> statements = statementTable.getStatements(st);
	std::vector<Node*> following;
	for(std::vector<Node*>::iterator it = statements.begin(); it != statements.end(); ++it) {
		Node* followee = getFollowing(*it);
		if (followee != nullptr) {
			following.push_back(followee);
		}
	}
	return following;
}

std::vector<Node*> PKB::getFollowedBy(int statementLine) {
	Node* statement = statementTable.getStatement(statementLine);
	std::vector<Node*> followedBy;
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
		return stmtLstNode->getStmtLst()[index - 1]; // magic number!!!
	}
	return nullptr;
}

std::vector<Node*> PKB::getFollowedBy(synt_type st) {
	std::vector<Node*> statements = statementTable.getStatements(st);
	std::vector<Node*> followers;
	for(std::vector<Node*>::iterator it = statements.begin(); it != statements.end(); ++it) {
		Node* follower = getFollowedBy(*it);
		if (follower != nullptr) {
			followers.push_back(follower);
		}
	}
	return followers;
}

std::vector<Node*> PKB::getExpressions(std::string expr) {
	return expressionTable.getExpressions(expr);
}

std::vector<Node*> PKB::getRootExpressions(std::string expr) {
	return expressionTable.getRootExpressions(expr);
}
