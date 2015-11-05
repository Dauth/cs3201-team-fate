#include "stdafx.h"
#include "PKB.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include "ExpressionTree.h"
#include "CFGNode.h"

using namespace std;

PKB::PKB(ExpressionTree* _et) {
	LIST_INDEX_OFFSET = 1;
	et = _et;
}

void PKB::buildCFG() {
	vector<Node*> procedures = procedureTable.getAllProcedures();
	for (int i=0; i<procedures.size(); i++) {
		cfg.buildCFG(procedures.at(i));
	}
}

vector<Node*> PKB::getNodes(SyntType st) {
	if(st == variable) {
		return variableTable.getNodes();
	} else if(st == constant) {
		std::vector<Node*> nodes;
		for(std::unordered_map<string, vector<Node*>>::iterator it = constants.begin(); it != constants.end(); ++it) {
			nodes.push_back(it->second.at(0));
		}
		return nodes;;
	}else if(st == procedure) {
		return procedureTable.getAllProcedures();
	} else if (st == progline) {
		return statementTable.getStatements(statement);
	}
	return statementTable.getStatements(st);
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
	if (st == constant) {
		if ( constants.find(node->getValue()) == constants.end() ) {
			constants[node->getValue()] = vector<Node*>();
		}
		constants[node->getValue()].push_back(node);
	}
	if (st == whileLoop || st == ifelse || st == assignment || st == call) {
		statementTable.addStatement(oss.str(), node);
		node->setRoot(procedure);
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
	if (parent != nullptr && node->isStatement()) {
		handleParent(node, parent);
	}
	if (st == call) {
		handleCalls(procedure, node, parent);
	}
	if (st == variable) {
		variableTable.addNode(node);
		handleInheritance(procedure, node, modifiedBy, usedBy);
	}
	return node;
}

void PKB::handleInheritance(Node* procNode, Node* node, Node* modifiedBy, Node* usedBy) {
	if (procNode != nullptr && callsTable.isCalled(procNode->getValue())) {
		vector<pair<string, string>> calls = getCallsStar(procedure, procNode->getValue());
		for (int i=0; i<calls.size(); i++) {
			Node* callingProc = procedureTable.getProcedure(calls[i].first);
			vector<string> callingStatements = callsTable.getCallingStatement(calls[i].first);
			vector<string> callingStatements2 = callsTable.getCallingStatement(procNode->getValue());
			callingStatements.insert(callingStatements.end(), callingStatements2.begin(), callingStatements2.end());
			if (modifiedBy != nullptr) {
				modifiesTable.addModifies(callingProc, node->getValue());
				for (int j=0; j<callingStatements.size(); j++) {
					Node* callsNode = statementTable.getStatement(callingStatements[j]);
					modifiesTable.addModifies(callsNode, node->getValue());
					Node* grandParent = callsNode->getParent()->getParent();
					while (grandParent != nullptr) {
						if (grandParent->getType() == whileLoop || grandParent->getType() == ifelse) {
							modifiesTable.addModifies(grandParent, node->getValue());
							grandParent = grandParent->getParent()->getParent();
						} else {
							grandParent = nullptr;
						}
					}
				}
			} else if (usedBy != nullptr) {
				usesTable.addUses(callingProc, node->getValue());
				for (int j=0; j<callingStatements.size(); j++) {
					Node* callsNode = statementTable.getStatement(callingStatements[j]);
					usesTable.addUses(callsNode, node->getValue());
					Node* grandParent = callsNode->getParent()->getParent();
					while (grandParent != nullptr) {
						if (grandParent->getType() == whileLoop || grandParent->getType() == ifelse) {
							usesTable.addUses(grandParent, node->getValue());
							grandParent = grandParent->getParent()->getParent();
						} else {
							grandParent = nullptr;
						}
					}
				}
			}
		}
	}
}

void PKB::handleCalls(Node* callingProc, Node* node, Node* parent) {
	callsTable.addCalls(callingProc, node);
	Node* targetProc = procedureTable.getProcedure(node->getValue());
	if (targetProc != nullptr ) {
		vector<pair<string, string>> calls = getCallsStar(targetProc->getValue(), procedure);;
		vector<pair<string, string>> callsInherit = getCallsStar(procedure, targetProc->getValue());
		calls.insert(calls.end(), callsInherit.begin(), callsInherit.end());
		for (int i=0; i<calls.size(); i++) {
			string calledProc = calls[i].second;
			vector<pair<string, string>> modifies = getModifies(calledProc, variable);
			vector<pair<string, string>> uses = getUses(calledProc, variable);
			vector<string> callingStatements = callsTable.getCallingStatement(calls[i].first);
			vector<string> callingStatements2 = callsTable.getCallingStatement(node->getValue());
			callingStatements.insert(callingStatements.end(), callingStatements2.begin(), callingStatements2.end());
			for(int j=0; j<modifies.size(); j++) {
				Node* proc = procedureTable.getProcedure(calls[i].first);
				modifiesTable.addModifies(proc, modifies[j].second);
				for (int k=0; k<callingStatements.size(); k++) {
					Node* callsNode = statementTable.getStatement(callingStatements[k]);
					modifiesTable.addModifies(callsNode, modifies[j].second);
					Node* grandParent = parent;
					if(callsNode != node) {
						grandParent = callsNode->getParent()->getParent();
					}
					while (grandParent != nullptr) {
						if (grandParent->getType() == whileLoop || grandParent->getType() == ifelse) {
							modifiesTable.addModifies(grandParent, modifies[j].second);
							grandParent = grandParent->getParent()->getParent();
						} else {
							grandParent = nullptr;
						}
					}
				}
				modifiesTable.addModifies(node, modifies[j].second);
			}
			for(int j=0; j<uses.size(); j++) {
				Node* proc = procedureTable.getProcedure(calls[i].first);
				usesTable.addUses(proc, uses[j].second);
				for (int k=0; k<callingStatements.size(); k++) {
					Node* callsNode = statementTable.getStatement(callingStatements[k]);
					usesTable.addUses(callsNode, uses[j].second);
					Node* grandParent = parent;
					if(callsNode != node) {
						grandParent = callsNode->getParent()->getParent();
					}
					while (grandParent != nullptr) {
						if (grandParent->getType() == whileLoop || grandParent->getType() == ifelse) {
							usesTable.addUses(grandParent, uses[j].second);
							grandParent = grandParent->getParent()->getParent();
						} else {
							grandParent = nullptr;
						}
					}
				}
				usesTable.addUses(node, uses[j].second);
			}
		}
	}
}

void PKB::handleModifiedBy(Node* node, Node* modifiedBy, Node* procedure, Node* parent) {
	modifiesTable.addModifies(modifiedBy, node->getValue());
	modifiesTable.addModifies(procedure, node->getValue());
	while (parent != nullptr) {
		modifiesTable.addModifies(parent, node->getValue());
		Node* grandParent = parent->getParent()->getParent();
		if (grandParent->getType() == whileLoop || grandParent->getType() == ifelse) {
			parent = grandParent;
		} else {
			parent = nullptr;
		}
	}
}

void PKB::handleUsedBy(Node* node, Node* usedBy, Node* procedure, Node* parent) {
	usesTable.addUses(usedBy, node->getValue());
	usesTable.addUses(procedure, node->getValue());
	while (parent != nullptr) {
		usesTable.addUses(parent, node->getValue());
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
	if (st1 == progline) st1 = statement;
	return modifiesTable.getByLeftKey(st1);
}

vector<pair<string, string>> PKB::getModifies(SyntType st, string varName) {
	if (st == progline) st = statement;
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
	if(modifiesTable.isModified(ident, varName)) {
		results.push_back(make_pair(ident, varName));
	}
	return results;
}

vector<pair<string, string>> PKB::getUses(SyntType st1, SyntType st2) {
	if (st1 == progline) st1 = statement;
	return usesTable.getByLeftKey(st1);
}

vector<pair<string, string>> PKB::getUses(SyntType st, string varName) {
	if (st == progline) st = statement;
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
				if (stmtNode != nullptr && ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType())) {
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
	if(usesTable.isUsed(ident, varName)) {
		results.push_back(make_pair(ident, varName));
	}
	return results;
}

vector<pair<string, string>> PKB::getParents(SyntType st1, SyntType st2) {
	if (st1 == progline) st1 = statement;
	if (st2 == progline) st2 = statement;
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
	if (st == progline) st = statement;
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
	if (st == progline) st = statement;
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
	if (st1 == progline) st1 = statement;
	if (st2 == progline) st2 = statement;
	set<pair<string, string>> results;
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
				string indirectRight = indirectParents[j].second;
				Node* indirectStmtNode = statementTable.getStatement(indirectRight);
				if ((indirectStmtNode->isStatement() && st2 == statement) || st2 == indirectStmtNode->getType()) {
					pair<string, string> parentsStar ( left, indirectRight );
					results.insert(parentsStar);
				}
			}
		} 
		if ((stmtNode->isStatement() && st2 == statement) || st2 == stmtNode->getType()) {
			results.insert(parents[i]);
		}
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

vector<pair<string, string>> PKB::getParentsStar(SyntType st, string stmtNum) {
	if (st == progline) st = statement;
	set<pair<string, string>> results;
	vector<pair<string, string>> parents = getParents(st, stmtNum);
	if (parents.empty()){
		return parents;
	}

	for (int i=0; i<parents.size(); i++) {
		string left = parents[i].first;
		string right = parents[i].second;
		Node* stmtNode = statementTable.getStatement(left);
		vector<pair<string, string>> indirectParents = getParentsStar(statement, left);
		for (int j=0; j<indirectParents.size(); j++) {
			string indirectLeft = indirectParents[j].first;
			Node* indirectStmtNode = statementTable.getStatement(indirectLeft);
			if ((indirectStmtNode->isStatement() && st == statement) || st == indirectStmtNode->getType()) {
				pair<string, string> parentsStar ( indirectLeft, right );
				results.insert(parentsStar);
			}
		}
		if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
			results.insert(parents[i]);
		}
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

vector<pair<string, string>> PKB::getParentsStar(string stmtNum, SyntType st) {
	if (st == progline) st = statement;
	set<pair<string, string>> results;
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
				string indirectRight = indirectParents[j].second;
				Node* indirectStmtNode = statementTable.getStatement(indirectRight);
				if ((indirectStmtNode->isStatement() && st == statement) || st == indirectStmtNode->getType()) {
					pair<string, string> parentsStar ( left, indirectRight );
					results.insert(parentsStar);
				}
			}
		}
		if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
			results.insert(parents[i]);
		}
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

vector<pair<string, string>> PKB::getParentsStar(string stmtNum1, string stmtNum2) {
	set<pair<string, string>> results;
	vector<pair<string, string>> parents = getParents(stmtNum1, statement);
	if (parents.empty()){
		return parents;
	}

	for (int i=0; i<parents.size(); i++) {
		string left = parents[i].first;
		string right = parents[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		if (right.compare(stmtNum2) == 0) {
			results.insert(parents[i]);
		} else if (stmtNode->isContainer()) {
			vector<pair<string, string>> indirectParents = getParentsStar(stmtNode->getLine(), statement);
			for (int j=0; j<indirectParents.size(); j++) {
				string indirectRight = indirectParents[j].second;
				if (indirectRight.compare(stmtNum2) == 0){
					pair<string, string> parentsStar ( left, indirectRight );
					results.insert(parentsStar);
				}
			}
		}
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}


vector<pair<string, string>> PKB::getFollows(SyntType st1, SyntType st2) {
	if (st1 == progline) st1 = statement;
	if (st2 == progline) st2 = statement;
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
	if (st == progline) st = statement;
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
	if (st == progline) st = statement;
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
		if (rightNode != nullptr && rightNode->getLine().compare(stmtNum2) == 0) {
			pair<string, string> follows ( leftNode->getLine(), rightNode->getLine() );
			results.push_back(follows);
		}
	}
	return results;
}

vector<pair<string, string>> PKB::getFollowsStar(SyntType st1, SyntType st2) {
	if (st1 == progline) st1 = statement;
	if (st2 == progline) st2 = statement;
	set<pair<string, string>> results;
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
			string indirectRight = indirectFollows[j].second;
			Node* indirectStmtNode = statementTable.getStatement(indirectRight);
			if ((indirectStmtNode->isStatement() && st2 == statement) || st2 == indirectStmtNode->getType()) {
				pair<string, string> followsStar ( left, indirectRight );
				results.insert(followsStar);
			}
		}
		if ((stmtNode->isStatement() && st2 == statement) || st2 == stmtNode->getType()) {
			results.insert(follows[i]);
		}
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

vector<pair<string, string>> PKB::getFollowsStar(SyntType st, string stmtNum) {
	if (st == progline) st = statement;
	set<pair<string, string>> results;
	vector<pair<string, string>> follows = getFollows(statement, stmtNum);
	if (follows.empty()){
		return follows;
	}

	for (int i=0; i<follows.size(); i++) {
		string left = follows[i].first;
		string right = follows[i].second;
		Node* stmtNode = statementTable.getStatement(left);
		vector<pair<string, string>> indirectFollows = getFollowsStar(statement, left);
		for (int j=0; j<indirectFollows.size(); j++) {
			string indirectLeft = indirectFollows[j].first;
			Node* indirectStmtNode = statementTable.getStatement(indirectLeft);
			if ((indirectStmtNode->isStatement() && st == statement) || st == indirectStmtNode->getType()) {
				pair<string, string> followsStar ( indirectLeft, right );
				results.insert(followsStar);
			}
		}
		if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
			results.insert(follows[i]);
		}
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

vector<pair<string, string>> PKB::getFollowsStar(string stmtNum, SyntType st) {
	if (st == progline) st = statement;
	set<pair<string, string>> results;
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
			string indirectRight = indirectFollows[j].second;
			Node* indirectStmtNode = statementTable.getStatement(indirectRight);
			if ((indirectStmtNode->isStatement() && st == statement) || st == indirectStmtNode->getType()) {
				pair<string, string> followsStar ( left, indirectRight );
				results.insert(followsStar);
			}
		}
		if ((stmtNode->isStatement() && st == statement) || st == stmtNode->getType()) {
			results.insert(follows[i]);
		}
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

vector<pair<string, string>> PKB::getFollowsStar(string stmtNum1, string stmtNum2) {
	set<pair<string, string>> results;
	vector<pair<string, string>> follows = getFollows(stmtNum1, statement);
	if (follows.empty()){
		return follows;
	}

	for (int i=0; i<follows.size(); i++) {
		string left = follows[i].first;
		string right = follows[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		if (right.compare(stmtNum2) == 0) {
			results.insert(follows[i]);
		} else {
			vector<pair<string, string>> indirectFollows = getFollowsStar(stmtNode->getLine(), statement);
			for (int j=0; j<indirectFollows.size(); j++) {
				string indirectRight = indirectFollows[j].second;
				if (indirectRight.compare(stmtNum2) == 0){
					pair<string, string> followsStar ( left, indirectRight );
					results.insert(followsStar);
				}
			}
		}
	}
	return vector<pair<string, string>>(results.begin(), results.end());
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
	set<pair<string, string>> results;
	vector<pair<string, string>> temp1 = callsTable.getByLeftKey(stmtNum1);
	vector<pair<string, string>> temp2 = callsTable.getByRightKey(stmtNum2);

	if (temp1.size() == 0 || temp2.size() == 0) {
		return vector<pair<string, string>>(results.begin(), results.end());
	} else if (temp1.size() < temp2.size()) {
		for(int i=0; i<temp1.size(); i++) {
			string right = temp1[i].second;
			if (right.compare(stmtNum2) == 0) {
				results.insert(temp1[i]);
			}
		}
	} else {
		for(int i=0; i<temp2.size(); i++) {
			string left = temp2[i].first;
			if (left.compare(stmtNum1) == 0) {
				results.insert(temp2[i]);
			}
		}
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

vector<pair<string, string>> PKB::getCallsStar(SyntType st1, SyntType st2) {
	set<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(st1, st2);
	if (calls.empty()){
		return calls;
	}

	for (int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;
		vector<pair<string, string>> indirectCalls = getCallsStar(right, st2);
		for (int j=0; j<indirectCalls.size(); j++) {
			string indirectRight = indirectCalls[j].second;
			pair<string, string> callsStar ( left, indirectRight );
			results.insert(callsStar);
		}
		results.insert(calls[i]);
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

vector<pair<string, string>> PKB::getCallsStar(SyntType st, string procName) {
	set<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(st, procName);
	if (calls.empty()){
		return calls;
	}

	for (int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;
		vector<pair<string, string>> indirectCalls = getCallsStar(st, left);
		for (int j=0; j<indirectCalls.size(); j++) {
			string indirectLeft = indirectCalls[j].first;
			pair<string, string> callsStar ( indirectLeft, right );
			results.insert(callsStar);
		}
		results.insert(calls[i]);
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

vector<pair<string, string>> PKB::getCallsStar(string procName, SyntType st) {
	set<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(procName, st);
	if (calls.empty()){
		return calls;
	}

	for (int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;
		vector<pair<string, string>> indirectCalls = getCallsStar(right, st);
		for (int j=0; j<indirectCalls.size(); j++) {
			string indirectRight = indirectCalls[j].second;
			pair<string, string> callsStar ( left, indirectRight );
			results.insert(callsStar);
		}
		results.insert(calls[i]);
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

vector<pair<string, string>> PKB::getCallsStar(string procName1, string procName2) {
	set<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(procName1, procedure);
	if (calls.empty()){
		return calls;
	}

	for (int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;;
		if (right.compare(procName2) == 0) {
			results.insert(calls[i]);
		} else {
			vector<pair<string, string>> indirectCalls = getCallsStar(right, procedure);
			for (int j=0; j<indirectCalls.size(); j++) {
				string indirectRight = indirectCalls[j].second;
				if (indirectRight.compare(procName2) == 0){
					pair<string, string> followsStar ( left, indirectRight );
					results.insert(followsStar);
				}
			}
		}
	}
	return vector<pair<string, string>>(results.begin(), results.end());
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
				if (whileList.at(i)->getLeftChild()->getValue().compare(left) == 0){
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
				if (ifList.at(i)->getLeftChild()->getValue().compare(left) == 0){
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
				results = getModifies(assignment, left);
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
						if(parentNode->getLeftChild()->getValue() == left){
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
					if(parentNode->getLeftChild()->getValue() == left){
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
							if(parentNode->getLeftChild()->getValue() == left){
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
						if(parentNode->getLeftChild()->getValue() == left){
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

vector<pair<string, string>> PKB::getNext(SyntType s1, SyntType s2){
	return cfg.getAllPairs();
}

vector<pair<string, string>> PKB::getNext(SyntType s1, string sNum2){
	vector<pair<string, string>> nextList;
	vector<CFGNode*> prevList = statementTable.getStatement(sNum2)->getCfgNode()->getPrevNode();
	for(unsigned i = 0; i < prevList.size(); i++){
		Node* temp = prevList.at(i)->getStatement();
		pair<string, string> nextPair (temp->getLine(),sNum2);
		nextList.push_back(nextPair);
	}

	return nextList;
}

vector<pair<string, string>> PKB::getNext(string sNum1, SyntType s2){
	vector<pair<string, string>> nextList;
	Node* temp = statementTable.getStatement(sNum1);
	if(temp->getType() == whileLoop || temp->getType() == ifelse){
		CFGNode* nextCFG1 = temp->getCfgNode()->getNextNode();
		CFGNode* nextCFG2 = temp->getCfgNode()->getExNextNode();
		if(nextCFG1){
			Node* next = nextCFG1->getStatement();
			pair<string, string> pair1 (sNum1, next->getLine());
			nextList.push_back(pair1);
		}

		if(nextCFG2){
			Node* next = nextCFG2->getStatement();
			pair<string, string> pair2 (sNum1, next->getLine());
			nextList.push_back(pair2);
		}
	}
	else{
		CFGNode* nextCFG = temp->getCfgNode()->getNextNode();
		if(nextCFG){
			Node* next = nextCFG->getStatement();
			pair<string, string> pair (sNum1, next->getLine());
			nextList.push_back(pair);
		}
	}
	
	return nextList;
}

vector<pair<string, string>> PKB::getNext(string sNum1, string sNum2){
	vector<pair<string, string>> nextList;
	Node* temp = statementTable.getStatement(sNum1);
	if(temp->getType() == whileLoop || temp->getType() == ifelse){
		Node* next1 = temp->getCfgNode()->getNextNode()->getStatement();
		Node* next2 = temp->getCfgNode()->getExNextNode()->getStatement();
		if ( next1->getLine() == sNum2){
			pair<string, string> pair1 (sNum1, next1->getLine());
			nextList.push_back(pair1);
		}
		if( next2->getLine() == sNum2){
			pair<string, string> pair2 (sNum1, next2->getLine());
			nextList.push_back(pair2);
		}
	}
	else{
		Node* next = temp->getCfgNode()->getNextNode()->getStatement();
		if ( next->getLine() == sNum2){
			pair<string, string> pair (sNum1, next->getLine());
			nextList.push_back(pair);
		}
	}
	
	return nextList;
}

vector<pair<string, string>> PKB::getNextStar(SyntType s1, SyntType s2){
	vector<pair<string, string>> results;
	for(int i = 1; i <= statementTable.getStatementCount(statement);i++){
		ostringstream oss ;
		oss << i;
		string lineNum = oss.str();
		vector<pair<string, string>> temp = getNextStar(lineNum,statement);
		results.insert(results.end(),temp.begin(),temp.end());
	}

	return results;
}
vector<pair<string, string>> PKB::getNextStar(SyntType s1, string num2){
	vector<pair<string, string>> results;
	Node* tempNode = statementTable.getStatement(num2);
	if ( tempNode->getType() == whileLoop){
		pair<string, string> nextPair (num2,num2);
		results.push_back(nextPair);
		vector<pair<string, string>> children = getParentsStar(num2,statement);
		for( int j = 0; j < children.size(); j++){
			pair<string, string> nextPair (children[j].second,num2);
			results.push_back(nextPair);
		}
	}

	vector<CFGNode*> prevNodes = tempNode->getCfgNode()->getPrevNode();
	for( int j = 0; j < prevNodes.size(); j++){
		Node* pNode = prevNodes[j]->getStatement();
		pair<string, string> prevPair (pNode->getLine(),num2);
		results.push_back(prevPair);
		if( atoi(pNode->getLine().c_str()) < atoi(num2.c_str())){
			vector<pair<string, string>> prevResults = getNextStar(s1,pNode->getLine());
			for ( int i = 0; i < prevResults.size(); i++){
				pair<string, string> prevPair (prevResults[i].first,num2);
				results.push_back(prevPair);
			}
		}
	}

	set<pair<string, string>> s (results.begin(),results.end());
	results.assign(s.begin(),s.end());
	return results;
}
vector<pair<string, string>> PKB::getNextStar(string num1, SyntType s2){
	vector<pair<string, string>> results;
	Node* tempNode = statementTable.getStatement(num1);
	if( tempNode->getType() == whileLoop || tempNode->getType() == ifelse){
		vector<pair<string, string>> childrens = getParentsStar(tempNode->getLine(),statement);
		for( int j = 0; j < childrens.size(); j++){
			pair<string, string> nextPair (num1,childrens[j].second);
			results.push_back(nextPair);
		}
		CFGNode* nextNode;
		if( tempNode->getType() == whileLoop){
			pair<string, string> nextPair (num1,tempNode->getLine());
			results.push_back(nextPair);
			nextNode = tempNode->getCfgNode()->getNextNode();
		}
		else{
			nextNode = tempNode->getCfgNode()->getEndLeft()->getNextNode();
		}
		if (nextNode){
			pair<string, string> nextPair (num1,nextNode->getStatement()->getLine());
			results.push_back(nextPair);
			vector<pair<string,string>> tempResults = getNextStar(nextNode->getStatement()->getLine(),s2);
			for(int i = 0; i < tempResults.size(); i++){
				pair<string, string> nextPair (num1,tempResults[i].second);
				results.push_back(nextPair);
			}
		}
	}

	else {
		CFGNode* nextNode = tempNode->getCfgNode()->getNextNode();
		if(nextNode){
			pair<string, string> nextPair (num1,nextNode->getStatement()->getLine());
			results.push_back(nextPair);
			vector<pair<string,string>> tempResults = getNextStar(nextNode->getStatement()->getLine(),s2);
			for(int i = 0; i < tempResults.size(); i++){
				pair<string, string> nextPair (num1,tempResults[i].second);
				results.push_back(nextPair);
			}
		}
	}

	set<pair<string, string>> s (results.begin(),results.end());
	results.assign(s.begin(),s.end());
	return results;
}
vector<pair<string, string>> PKB::getNextStar(string num1, string num2){
	vector<pair<string, string>> results;
	vector<pair<string, string>> nextStar = getNextStar(num1,statement);
	for( int i = 0; i < nextStar.size(); i++){
		if(nextStar[i].second == num2){
			pair<string, string> nextPair (num1,num2);
			results.push_back(nextPair);
			break;
		}
	}

	return results;
}

vector<pair<string, string>> PKB::getAffects(SyntType st1, SyntType st2) {
	vector<pair<string, string>> results;
	vector<Node*> assignments = statementTable.getStatements(assignment);
	for (int i=0; i<assignments.size(); i++) {
		vector<pair<string, string>> affects = getAffects(assignments[i]->getLine(), assignment);
		results.insert(results.end(), affects.begin(), affects.end());
	}
	return results;
}

vector<pair<string, string>> PKB::getAffects(SyntType st, string assign) {
	set<pair<string, string>> results;
	Node* assignnode = statementTable.getStatement(assign);
	if(assignnode->getType() != assignment) {
		return vector<pair<string, string>>();
	}
	vector<pair<string, string>> uses = getUses(assign, variable);
	for (int i=0; i<uses.size(); i++) {
		if(modifiesTable.isModified(assignnode->getRoot()->getValue(), uses[i].second)) {
			set<pair<string, Node*>> searched;
			reverseAffectsSearch(&results, &searched, assignnode, assign, uses[i].second, false);
		}
	}
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> PKB::getAffects(string assign, SyntType st) {
	set<pair<string, string>> results;
	Node* assignnode = statementTable.getStatement(assign);
	if(assignnode->getType() != assignment) {
		return vector<pair<string, string>>();
	}
	string varName = assignnode->getLeftChild()->getValue();
	if(!usesTable.isUsed(assignnode->getRoot()->getValue(), varName)) {
		return vector<pair<string, string>>();
	}
	set<pair<string, Node*>> searched;
	forwardAffectsSearch(&results, &searched, assignnode, assign, varName, "", false);
	return vector<pair<string, string>> (results.begin(), results.end()); 
}

vector<pair<string, string>> PKB::getAffects(string assign1, string assign2) {
	set<pair<string, string>> results;
	Node* assign1node = statementTable.getStatement(assign1);
	Node* assign2node = statementTable.getStatement(assign2);
	if (assign1node->getType() != assignment || assign2node->getType() != assignment) {
		return vector<pair<string, string>>();
	}
	if (assign1node->getRoot() != assign2node->getRoot()) {
		return vector<pair<string, string>>();
	}
	string varName = assign1node->getLeftChild()->getValue();
	if(!usesTable.isUsed(assign2node->getRoot()->getValue(), varName)) {
		return vector<pair<string, string>>();
	}
	set<pair<string, Node*>> searched;
	forwardAffectsSearch(&results, &searched, assign1node, assign1, varName, assign2, false);
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> PKB::getAffectsStar(SyntType st1, SyntType st2) {
	vector<pair<string, string>> results;
	vector<Node*> assignments = statementTable.getStatements(assignment);
	for (int i=0; i<assignments.size(); i++) {
		vector<pair<string, string>> affects = getAffectsStar(assignments[i]->getLine(), assignment);
		results.insert(results.end(), affects.begin(), affects.end());
	}
	return results;
}

vector<pair<string, string>> PKB::getAffectsStar(SyntType st, string assign) {
	set<pair<string, string>> results;
	Node* assignnode = statementTable.getStatement(assign);
	if(assignnode->getType() != assignment) {
		return vector<pair<string, string>>();
	}
	vector<pair<string, string>> uses = getUses(assign, variable);
	for (int i=0; i<uses.size(); i++) {
		if(modifiesTable.isModified(assignnode->getRoot()->getValue(), uses[i].second)) {
			set<pair<string, Node*>> searched;
			reverseAffectsSearch(&results, &searched, assignnode, assign, uses[i].second, true);
		}
	}
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> PKB::getAffectsStar(string assign, SyntType st) {
	set<pair<string, string>> results;
	Node* assignnode = statementTable.getStatement(assign);
	if(assignnode->getType() != assignment) {
		return vector<pair<string, string>>();
	}
	string varName = assignnode->getLeftChild()->getValue();
	if(!usesTable.isUsed(assignnode->getRoot()->getValue(), varName)) {
		return vector<pair<string, string>>();
	}
	set<pair<string, Node*>> searched;
	forwardAffectsSearch(&results, &searched, assignnode, assign, varName, "", true);
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> PKB::getAffectsStar(string assign1, string assign2) {
	set<pair<string, string>> results;
	Node* assign1node = statementTable.getStatement(assign1);
	Node* assign2node = statementTable.getStatement(assign2);
	if (assign1node->getType() != assignment || assign2node->getType() != assignment) {
		return vector<pair<string, string>>();
	}
	if (assign1node->getRoot() != assign2node->getRoot()) {
		return vector<pair<string, string>>();
	}
	string varName = assign1node->getLeftChild()->getValue();
	if(!usesTable.isUsed(assign2node->getRoot()->getValue(), varName)) {
		return vector<pair<string, string>>();
	}
	set<pair<string, Node*>> searched;
	forwardAffectsSearch(&results, &searched, assign1node, assign1, varName, assign2, true);
	return vector<pair<string, string>> (results.begin(), results.end());
}

void PKB::forwardAffectsSearch(set<pair<string, string>>* results, set<pair<string, Node*>>* searched, Node* node, string origin, string varName, string stop, bool indirect) {
	vector<pair<string, string>> next = getNext(node->getLine(), progline);
	for(int i=0; i<next.size(); i++) {
		Node* stmt = statementTable.getStatement(next[i].second);
		if(searched->find(make_pair(varName, stmt)) == searched->end()) {
			searched->insert(make_pair(varName, stmt));
			if((stmt->getType() == assignment && usesTable.isUsed(stmt->getLine(), varName))) {
				if(stop == "" || (stop != "" && stop == stmt->getLine())) {
					results->insert(make_pair(origin, stmt->getLine()));
				}
				if(stop != stmt->getLine()) {
					forwardAffectsSearch(results, searched, stmt, origin, varName, stop, indirect);
				}
				if (indirect) { 
					forwardAffectsSearch(results, searched, stmt, origin, stmt->getLeftChild()->getValue(), stop, indirect);
				}
			} else if ((stmt->getType() == assignment && stmt->getLeftChild()->getValue() != varName)
					|| (stmt->getType() == call && !modifiesTable.isModified(stmt->getLine(), varName))
					|| (stmt->getType() != call && stmt->getType() != assignment)){
				forwardAffectsSearch(results, searched, stmt, origin, varName, stop, indirect);
			}
		}
	}
}

void PKB::reverseAffectsSearch(set<pair<string, string>>* results, set<pair<string, Node*>>* searched, Node* node, string origin, string varName, bool indirect) {
	vector<pair<string, string>> prev = getNext(progline, node->getLine());
	for(int i=0; i<prev.size(); i++) {
		Node* stmt = statementTable.getStatement(prev[i].first);
		if(searched->find(make_pair(varName, stmt)) == searched->end()) {
			searched->insert(make_pair(varName, stmt));
			if((stmt->getType() == assignment && modifiesTable.isModified(stmt->getLine(), varName))) {
				results->insert(make_pair(stmt->getLine(), origin));
				if (indirect) {
					vector<pair<string, string>> uses = getUses(stmt->getLine(), variable);
					for (int j=0; j<uses.size(); j++) {
						reverseAffectsSearch(results, searched, stmt, origin, uses[j].second, indirect);
					}
				}
			} else if ((stmt->getType() == assignment && !modifiesTable.isModified(stmt->getLine(), varName))
					|| (stmt->getType() == call && !modifiesTable.isModified(stmt->getLine(), varName))
					|| (stmt->getType() != call && stmt->getType() != assignment)) {
				reverseAffectsSearch(results, searched, stmt, origin, varName, indirect);
			}
		}
	}
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