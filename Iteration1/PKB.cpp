#include "stdafx.h"
#include "PKB.h"

PKB::PKB () {

}

std::vector<Node*> PKB::getStatement(synt_type st) {
	return statementTable.getStatements(st);
}
	
Node* PKB::createProcedure(std::string procName) {
	return procedureTable.getOrCreateProcedure(procName);
}

Node* PKB::createNode(synt_type st, int line, std::string value="", 
	Node* usedBy=nullptr, Node* modifiedBy=nullptr, Node* parent=nullptr, Node* procedure=nullptr) {
	Node* node = new Node(st, line, value);
	if(st == variable) {
		Variable* var = variableTable.getOrCreateVariable(value);
		node->setVar(var);
	}
	if (st == whileLoop || st == ifelse || st == assignment || st == call) {
		statementTable.addStatement(line, node);
	} else if (st == expression ) {
		expressionTable.addExpression(node);
	}

	if (modifiedBy != nullptr) {
		handleModifiedBy(node, usedBy, procedure);
	}
	if (usedBy != nullptr) {
		handleUsedBy(node, usedBy, procedure);
	}
	if (parent != nullptr) {
		handleParent(node, usedBy);
	}
	return node;
}

void PKB::handleModifiedBy(Node* node, Node* modifiedBy, Node* procedure) {
	modifiesTable.addModifies(modifiedBy, node);
	modifiesTable.addModifies(procedure, node);
	variableTable.addModifiedBy(node->getVariable()->getName(), modifiedBy);
	variableTable.addModifiedBy(procedure->getValue(), modifiedBy);
}

void PKB::handleUsedBy(Node* node, Node* usedBy, Node* procedure) {
	usesTable.addUsedBy(usedBy, node);
	usesTable.addUsedBy(procedure, node);
	variableTable.addUsedBy(node->getVariable()->getName(), usedBy);
	variableTable.addUsedBy(procedure->getValue(), usedBy);
}

void PKB::handleParent(Node* child, Node* parent) {
	parentsTable.addChild(parent, child);
}

std::vector<Node*> PKB::getModifies(std::string procName) {
	Node* procedure = procedureTable.getOrCreateProcedure(procName);
	return modifiesTable.getModified(procedure);
}

std::vector<Node*> PKB::getModifies(int statementLine) {
	Node* node = statementTable.getStatement(statementLine);
	return modifiesTable.getModified(node);
}

std::vector<Node*> PKB::getModifies(synt_type st) {
	std::vector<Node*> modifiers = modifiesTable.getModifiers(st);
	std::set<Node*> modifies;
	for(std::vector<Node*>::iterator it = modifiers.begin(); it != modifiers.end(); ++it) {
		std::vector<Node*> temp = modifiesTable.getModified(*it);
		for(std::vector<Node*>::iterator it2 = temp.begin(); it2 != temp.end(); ++it) {
			modifies.insert(*it2);
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
	Node* procedure = procedureTable.getOrCreateProcedure(procName);
	return usesTable.getUsed(procedure);
}

std::vector<Node*> PKB::getUses(int statementLine) {
	Node* node = statementTable.getStatement(statementLine);
	return usesTable.getUsed(node);
}

std::vector<Node*> PKB::getUses(synt_type st) {
	std::vector<Node*> users = usesTable.getUsers(st);
	std::set<Node*> uses;
	for(std::vector<Node*>::iterator it = users.begin(); it != users.end(); ++it) {
		std::vector<Node*> temp = usesTable.getUsed(*it);
		for(std::vector<Node*>::iterator it2 = temp.begin(); it2 != temp.end(); ++it) {
			uses.insert(*it2);
		}
	}
	return std::vector<Node*> (uses.begin(), uses.end());
}

std::vector<Node*> PKB::getUsedBy(std::string varName) {
	Variable* variable = variableTable.getOrCreateVariable(varName);
	return variable->getUsedBy();
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
	return parentsTable.getChildren(parent);
}

std::vector<Node*> PKB::getChildren(synt_type st) {
	std::vector<Node*> parents = statementTable.getStatements(st);
	std::set<Node*> children;
	for(std::vector<Node*>::iterator it = parents.begin(); it != parents.end(); ++it) {
		std::vector<Node*> temp = parentsTable.getChildren(*it);
		for(std::vector<Node*>::iterator it2 = temp.begin(); it2 != temp.end(); ++it) {
			children.insert(*it2);
		}
	}
	return std::vector<Node*> (children.begin(), children.end());
}

std::vector<Node*> PKB::getParent(int statementLine) {
	Node* child = statementTable.getStatement(statementLine);
	return std::vector<Node*>(1, child->getParent()->getParent());
}

std::vector<Node*> PKB::getParents(synt_type st) {
	std::vector<Node*> children = statementTable.getStatements(st);
	std::set<Node*> parents;
	for(std::vector<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
		parents.insert((*it)->getParent()->getParent());
	}
	return std::vector<Node*> (children.begin(), children.end());
}

std::vector<Node*> PKB::getFollowing(int statementLine) {
	Node* statement = statementTable.getStatement(statementLine);
	Node* node = getFollowing(statement);
	if(node != nullptr) {
		return std::vector<Node*>(0, node);
	}
	return std::vector<Node*>();
}

//private
Node* PKB::getFollowing(Node* statement) {
	Node* stmtLstNode = statement->getParent();
	int index = stmtLstNode->getIndexLst()[statement];
	if(index < stmtLstNode->getIndexLst().size()) {
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
	Node* node = getFollowedBy(statement);
	if(node != nullptr) {
		return std::vector<Node*>(0, node);
	}
	return std::vector<Node*>();
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