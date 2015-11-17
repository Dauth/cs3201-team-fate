#include "stdafx.h"
#include "PKBFacade.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include "ExpressionTree.h"
#include "CFGNode.h"

using namespace std;

const int LIST_INDEX_OFFSET = 1;
const string NO_STOP = "";

// the PKBFacade acts as the facade for the entire PKB component and provides
// public APIs for the creation of nodes for the AST, as well as for getting
// design abtractions based on the information provided during node creation
// Node creation should only be used by the AST class, and the API calls for
// getting design abstraction should be called by the QueryEvaluator class.
// The PKBFacade requires a reference to the expressionTree class. While the
// expressionTree class is mainly used by the AST class for AST construction,
// The PKBFacade makes use of it for pattern matching
PKBFacade::PKBFacade(ExpressionTree* _et) {
	et = _et;
}

// This method builds the CFG after the AST has been constructed during the
// first pass of the simple source. It should only be called after the first
// pass has been completed.
void PKBFacade::buildCFG() {
	vector<Node*> procedures = procedureTable.getAllProcedures();
	for (unsigned int i=0; i<procedures.size(); i++) {
		cfg.buildCFG(procedures.at(i));
	}
}

// if returns nullpt procedure already exists
Node* PKBFacade::createProcedure(string procName) {
	return procedureTable.createProcedure(procName);
}

// This provides a faux factory method for the Simple Parser's AST to create
// new Nodes for every node that needs to be created in the AST. At the same time,
// the method also takes in information that can be used to infer design abstractions
// about the node being created. 
//
// In order for a node to be created, the following information must be provided
// Mandatory :
//			  Node Type (SyntType)
//			  Statement Line Number (int)
//			  Variable name, Constant name or Procedure name (string)
// Optional  :
//            Pointer to the node that uses this node (Node*)
//            Pointer to the node that modifies this node (Node*)
//            Pointer to the node that is this node's parent (Node*)
//            Pointer to the procedure node that this node belongs to (Node*)
//
// the method will then call the appropriate methods based on the provided
// inputs to precalculate the design abstractions and store them.
// It will then return the newly created Node object's pointer
Node* PKBFacade::createNode(SyntType st, int line, string value, 
	Node* usedBy, Node* modifiedBy, Node* parent, Node* procedure) {
	ostringstream oss;
	oss << line;
	Node* node = new Node(st, oss.str(), value);

	// adds a node to the statement table if its a statement
	// at the same time add the pointer to the procedure it belongs to
	if (node->isStatement()) {
		statementTable.addStatement(oss.str(), node);
		node->setRoot(procedure);
	// adds a node to the constant table if its a constant
	} else if (st == constant) {
		if ( constants.find(node->getValue()) == constants.end() ) {
			constants[node->getValue()] = vector<Node*>();
		}
		constants[node->getValue()].push_back(node);
	// adds a node to the variable table if its a variable
	// also calls the method to handle inheritance
	// this method checks to see if this variable needs to pass on
	// modifies and uses abstractions through calls 
	} else 	if (st == variable) {
		variableTable.addNode(node);
		handleInheritance(procedure, node, modifiedBy, usedBy);
	}
	// if the node is an expression, constant or variable that is on the right hand
	// side of an assignment statement, it is inserted into the expression table
	// this is used to speed up pattern matching
	if ((st == expression || st == constant || st == variable ) && usedBy != nullptr && usedBy->getType() == assignment) {
		expressionTable.addExpression(node);
		node->setExpParent(usedBy);
	}

	// handles modifies abstractions if the variable node is being modified
	if (modifiedBy != nullptr && st == variable) {
		handleModifiedBy(node, modifiedBy, procedure, parent);
	}
	// handles uses abstractions if the variable is being used
	if (usedBy != nullptr && st == variable) {
		handleUsedBy(node, usedBy, procedure, parent);
	}
	// handles parent abstraction if the node has a parent
	if (parent != nullptr && node->isStatement()) {
		handleParent(node, parent);
	}
	// handles call abstraction if the node is a call
	if (st == call) {
		handleCalls(procedure, node, parent);
	}
	return node;
}

// This methods provides the Query Evaluator a means of retrieving a set of 
// statements based on their statement type or variable. It is used when Synonyms
// that are Selected in a query are not part of any design abstraction. It also
// provides the means of retrieving constant nodes.
vector<Node*> PKBFacade::getNodes(SyntType st) {
	if (st == variable) {
		return variableTable.getNodes();
	} else if (st == constant) {
		std::vector<Node*> nodes;
		for (std::unordered_map<string, vector<Node*>>::iterator it = constants.begin(); it != constants.end(); ++it) {
			nodes.push_back(it->second.at(0));
		}
		return nodes;;
	} else if (st == procedure) {
		return procedureTable.getAllProcedures();
	} else if (st == progline) {
		return statementTable.getStatements(statement);
	}
	return statementTable.getStatements(st);
}

// method for Modifies abstraction with 2 unknowns
// retrieves the stored precomputed modifies based on the left
// synonym type. 
vector<pair<string, string>> PKBFacade::getModifies(SyntType st1, SyntType st2) {
	if (st1 == progline) {
		st1 = statement;
	}
	return modifiesTable.getByLeftKey(st1);
}

// method for Modifies abstraction with the variable known
// retrieves the stored precomputed modifies based on the known variable
// and the type of synonym provided. 
vector<pair<string, string>> PKBFacade::getModifies(SyntType st, string varName) {
	if (st == progline) {
		st = statement;
	}
	return modifiesTable.getByRightKey(varName, st);
}

// method for Modifies abstraction with the procedure or statement num known
// retrieves the stored precomputed modifies based on the known ident which
// can be a statement number or procedure name. 
vector<pair<string, string>> PKBFacade::getModifies(string ident, SyntType st) {
	return modifiesTable.getByLeftKey(ident);
}

// method for Modifies abstraction to check if the provided abstraction holds
// calls modifiesTable to check if the abstraction exists. 
vector<pair<string, string>> PKBFacade::getModifies(string ident, string varName) {
	vector<pair<string, string>> results;
	if (modifiesTable.isModified(ident, varName)) {
		results.push_back(make_pair(ident, varName));
	}
	return results;
}

// method for Uses abstraction with 2 unknowns
// retrieves the stored precomputed uses based on the left
// synonym type. 
vector<pair<string, string>> PKBFacade::getUses(SyntType st1, SyntType st2) {
	if (st1 == progline) {
		st1 = statement;
	}
	return usesTable.getByLeftKey(st1);
}

// method for Uses abstraction with the variable known
// retrieves the stored precomputed uses based on the known variable
// and the type of synonym provided. 
vector<pair<string, string>> PKBFacade::getUses(SyntType st, string varName) {
	if (st == progline) {
		st = statement;
	}
	return usesTable.getByRightKey(varName, st);
}

// method for Uses abstraction to check if the provided abstraction holds
// calls usesTable to check if the abstraction exists. O(1)
vector<pair<string, string>> PKBFacade::getUses(string ident, SyntType st) {
	return usesTable.getByLeftKey(ident);
}

// method for Uses abstraction with 2 unknowns
// retrieves the stored precomputed uses based on the left
// synonym type. 
vector<pair<string, string>> PKBFacade::getUses(string ident, string varName) {
	vector<pair<string, string>> results;
	if (usesTable.isUsed(ident, varName)) {
		results.push_back(make_pair(ident, varName));
	}
	return results;
}

// Method for Parent abstraction with 2 unknowns
// retrieves the stored precomputed parents abstraction based on both
// Synonym types provided. O
vector<pair<string, string>> PKBFacade::getParents(SyntType st1, SyntType st2) {
	if (st1 == progline) {
		st1 = statement;
	}
	if (st2 == progline) {
		st2 = statement;
	}
	return parentsTable.getByLeftKey(st1, st2);
}

// Method for Parent abstraction with the child statement known
// retrieves the stored precomputed abstraction based on the known child
// and the Parents's statement type. 
vector<pair<string, string>> PKBFacade::getParents(SyntType st, string stmtNum) {
	if (st == progline) {
		st = statement;
	}
	return parentsTable.getByRightKey(stmtNum, st);
}

// Method for Parent abstraction with the parent statement known
// retrieves the stored precomputed abstractions based on the known parent
// and the Child's statement type.
vector<pair<string, string>> PKBFacade::getParents(string stmtNum, SyntType st) {
	if (st == progline) {
		st = statement;
	}
	return parentsTable.getByLeftKey(stmtNum, st);
}

// Method for Parent abstraction with parent and child statements known
// calls the Parents table to check if the abstraction exists
vector<pair<string, string>> PKBFacade::getParents(string stmtNum1, string stmtNum2) {
	vector<pair<string, string>> results;
	if (parentsTable.isParent(stmtNum1, stmtNum2)) {
		results.push_back(make_pair(stmtNum1, stmtNum2));
	}
	return results;
}

// Method for Parent* abstraction with 2 unknowns
// recursively calls the method for getting Parent* with the parent statement known
// for every child of the unknown parents statement type, which will return every
// transitive child, and append those which match the type of the second unknown to
// the result vector
vector<pair<string, string>> PKBFacade::getParentsStar(SyntType st1, SyntType st2) {
	if (st1 == progline) {
		st1 = statement;
	}
	if (st2 == progline) {
		st2 = statement;
	}
	set<pair<string, string>> results;
	vector<pair<string, string>> parents = getParents(st1, statement);
	if (parents.empty()){
		return parents;
	}

	for (unsigned int i=0; i<parents.size(); i++) {
		string left = parents[i].first;
		string right = parents[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		if (stmtNode->isContainer()) {
			vector<pair<string, string>> indirectParents = getParentsStar(stmtNode->getLine(), statement);
			for (unsigned int j=0; j<indirectParents.size(); j++) {
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

// Method for Parent* abstraction with the child statement known
// recursively calls itself to every transitive parent the child has
// and append those which match the type of the second unknown to
// the result vector
vector<pair<string, string>> PKBFacade::getParentsStar(SyntType st, string stmtNum) {
	if (st == progline) {
		st = statement;
	}
	set<pair<string, string>> results;
	vector<pair<string, string>> parents = getParents(st, stmtNum);
	if (parents.empty()){
		return parents;
	}

	for (unsigned int i=0; i<parents.size(); i++) {
		string left = parents[i].first;
		string right = parents[i].second;
		Node* stmtNode = statementTable.getStatement(left);
		vector<pair<string, string>> indirectParents = getParentsStar(statement, left);
		for (unsigned int j=0; j<indirectParents.size(); j++) {
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

// Method for Parent* abstraction with the parent statement known
// recursively calls the method for getting Parent* with the parent statement known
// for every child of the known parents statement type, which will return its
// transitive children, and append those which match the type of the second unknown to
// the result vector
vector<pair<string, string>> PKBFacade::getParentsStar(string stmtNum, SyntType st) {
	if (st == progline) {
		st = statement;
	}
	set<pair<string, string>> results;
	vector<pair<string, string>> parents = getParents(stmtNum, statement);
	if (parents.empty()){
		return parents;
	}

	for (unsigned int i=0; i<parents.size(); i++) {
		string left = parents[i].first;
		string right = parents[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		if (stmtNode->isContainer()) {
			vector<pair<string, string>> indirectParents = getParentsStar(stmtNode->getLine(), statement);
			for (unsigned int j=0; j<indirectParents.size(); j++) {
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

// Method for Parent* abstraction parent and child statement known
// recursively calls the method for getting Parent* with the parent statement known
// for every child of the known parents statement type, which will return its
// transitive children, and find the child which matches the known child statement
vector<pair<string, string>> PKBFacade::getParentsStar(string stmtNum1, string stmtNum2) {
	set<pair<string, string>> results;
	vector<pair<string, string>> parents = getParents(stmtNum1, statement);
	if (parents.empty()){
		return parents;
	}

	for (unsigned int i=0; i<parents.size(); i++) {
		string left = parents[i].first;
		string right = parents[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		if (right.compare(stmtNum2) == 0) {
			results.insert(parents[i]);
		} else if (stmtNode->isContainer()) {
			vector<pair<string, string>> indirectParents = getParentsStar(stmtNode->getLine(), statement);
			for (unsigned int j=0; j<indirectParents.size(); j++) {
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

// Method for Follows with 2 unknowns
// Retrieves all statements for both unknown types
// Using the smaller result set, it checks for either
// the follower or followee and see if it matches the other
// unknown statement's type
vector<pair<string, string>> PKBFacade::getFollows(SyntType st1, SyntType st2) {
	if (st1 == progline) {
		st1 = statement;
	}
	if (st2 == progline) {
		st2 = statement;
	}
	vector<pair<string, string>> results;
	vector<Node*> temp1 = statementTable.getStatements(st1);
	vector<Node*> temp2 = statementTable.getStatements(st2);

	if (temp1.size() == 0 || temp2.size() == 0) {
		return results;
	} else if (temp1.size() < temp2.size()) {
		for (unsigned int i=0; i<temp1.size(); i++) {
			string left = temp1[i]->getLine();
			Node* stmtNode = statementTable.getStatement(left);
			Node* rightNode = getFollowee(stmtNode);
			if (rightNode != nullptr && (((rightNode->isStatement() && st2 == statement)) || st2 == rightNode->getType())) {
				pair<string, string> follows ( left, rightNode->getLine() );
				results.push_back(follows);
			}
		}
	} else {
		for (unsigned int i=0; i<temp2.size(); i++) {
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

// Method for Follows with followee statement known
// Retrieves known statement node from table, and finds the node
// that follows the known node
vector<pair<string, string>> PKBFacade::getFollows(SyntType st, string stmtNum) {
	if (st == progline) {
		st = statement;
	}
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

// Method for Follows with the follower statement known
// retrieves known statement node from table, and finds the node
// that it followers
vector<pair<string, string>> PKBFacade::getFollows(string stmtNum, SyntType st) {
	if (st == progline) {
		st = statement;
	}
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

// Method for Follows with both statements known
// uses the left known statement and find if the node that
// it follows matches the provided known followee statement
vector<pair<string, string>> PKBFacade::getFollows(string stmtNum1, string stmtNum2) {
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

// Method for Follows* abstraction with 2 unknowns
// recursively calls the method for getting Follows* with the follower statement known
// for every child of the unknown follower statement type, which will return every
// transitive followee, and append those which match the type of the second unknown to
// the result vector
vector<pair<string, string>> PKBFacade::getFollowsStar(SyntType st1, SyntType st2) {
	if (st1 == progline) {
		st1 = statement;
	}
	if (st2 == progline) {
		st2 = statement;
	}
	set<pair<string, string>> results;
	vector<pair<string, string>> follows = getFollows(st1, statement);
	if (follows.empty()){
		return follows;
	}

	for (unsigned int i=0; i<follows.size(); i++) {
		string left = follows[i].first;
		string right = follows[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		vector<pair<string, string>> indirectFollows = getFollowsStar(stmtNode->getLine(), statement);
		for (unsigned int j=0; j<indirectFollows.size(); j++) {
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

// Method for Follows* abstraction with the followee statement known
// recursively calls itself to every transitive follower the followee has
// and append those which match the type of the second unknown to
// the result vector
vector<pair<string, string>> PKBFacade::getFollowsStar(SyntType st, string stmtNum) {
	if (st == progline) {
		st = statement;
	}
	set<pair<string, string>> results;
	vector<pair<string, string>> follows = getFollows(statement, stmtNum);
	if (follows.empty()){
		return follows;
	}

	for (unsigned int i=0; i<follows.size(); i++) {
		string left = follows[i].first;
		string right = follows[i].second;
		Node* stmtNode = statementTable.getStatement(left);
		vector<pair<string, string>> indirectFollows = getFollowsStar(statement, left);
		for (unsigned int j=0; j<indirectFollows.size(); j++) {
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

// Method for Follows* abstraction with the follower statement known
// recursively calls the method for getting Follow* with the follower statement known
// for every followee of the known parents statement type, which will return its
// transitive followees, and append those which match the type of the second unknown to
// the result vector
vector<pair<string, string>> PKBFacade::getFollowsStar(string stmtNum, SyntType st) {
	if (st == progline) {
		st = statement;
	}
	set<pair<string, string>> results;
	vector<pair<string, string>> follows = getFollows(stmtNum, statement);
	if (follows.empty()){
		return follows;
	}

	for (unsigned int i=0; i<follows.size(); i++) {
		string left = follows[i].first;
		string right = follows[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		vector<pair<string, string>> indirectFollows = getFollowsStar(stmtNode->getLine(), statement);
		for (unsigned int j=0; j<indirectFollows.size(); j++) {
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

// Method for Follow* abstraction follower and followee statement known
// recursively calls the method for getting Follows* with the follower statement known
// for every followee of the known follower statement type, which will return its
// transitive followees, and find the followee which matches the known followee statement
vector<pair<string, string>> PKBFacade::getFollowsStar(string stmtNum1, string stmtNum2) {
	set<pair<string, string>> results;
	vector<pair<string, string>> follows = getFollows(stmtNum1, statement);
	if (follows.empty()){
		return follows;
	}

	for (unsigned int i=0; i<follows.size(); i++) {
		string left = follows[i].first;
		string right = follows[i].second;
		Node* stmtNode = statementTable.getStatement(right);
		if (right.compare(stmtNum2) == 0) {
			results.insert(follows[i]);
		} else {
			vector<pair<string, string>> indirectFollows = getFollowsStar(stmtNode->getLine(), statement);
			for (unsigned int j=0; j<indirectFollows.size(); j++) {
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

// Method for Calls with 2 unknowns
// get all calls, as there is only procedure type for both unknowns
vector<pair<string, string>> PKBFacade::getCalls(SyntType st1, SyntType st2) {
	return callsTable.getAll();
}

// Method for Calls with called procedure known
// get the procedure that calls the known procedure
vector<pair<string, string>> PKBFacade::getCalls(SyntType st, string stmtNum) {
	return callsTable.getByRightKey(stmtNum);
}

// Method for Calls with calling procedure known
// get the procedure that the provided procedure calls
vector<pair<string, string>> PKBFacade::getCalls(string stmtNum, SyntType st) {
	return callsTable.getByLeftKey(stmtNum);
}

// Method for calls with both procedures known
// Uses calls table to check if abstraction exists
vector<pair<string, string>> PKBFacade::getCalls(string stmtNum1, string stmtNum2) {
	vector<pair<string, string>> results;
	if (callsTable.isCalled(stmtNum1, stmtNum2)) {
		results.push_back(make_pair(stmtNum1, stmtNum2));
	}
	return results;
}

// Method for Calls* abstraction with 2 unknowns
// recursively calls the method for getting Calls* with the calling procedure known
// for every called procedure which will return every
// transitive called procedure, and append those to the result vector
vector<pair<string, string>> PKBFacade::getCallsStar(SyntType st1, SyntType st2) {
	set<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(st1, st2);
	if (calls.empty()){
		return calls;
	}

	for (unsigned int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;
		vector<pair<string, string>> indirectCalls = getCallsStar(right, st2);
		for (unsigned int j=0; j<indirectCalls.size(); j++) {
			string indirectRight = indirectCalls[j].second;
			pair<string, string> callsStar ( left, indirectRight );
			results.insert(callsStar);
		}
		results.insert(calls[i]);
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

// Method for Calls* abstraction with the called procedure known
// recursively calls itself to every transitive calling procedure the called procedure has
// and append those to the result vector
vector<pair<string, string>> PKBFacade::getCallsStar(SyntType st, string procName) {
	set<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(st, procName);
	if (calls.empty()){
		return calls;
	}

	for (unsigned int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;
		vector<pair<string, string>> indirectCalls = getCallsStar(st, left);
		for (unsigned int j=0; j<indirectCalls.size(); j++) {
			string indirectLeft = indirectCalls[j].first;
			pair<string, string> callsStar ( indirectLeft, right );
			results.insert(callsStar);
		}
		results.insert(calls[i]);
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

// Method for Calls* abstraction with the calling procedure known
// recursively calls the method for getting Calls* with the calling procedure known
// for every called procedure which will return its transitive called procedures
// and append those to the result vector
vector<pair<string, string>> PKBFacade::getCallsStar(string procName, SyntType st) {
	set<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(procName, st);
	if (calls.empty()){
		return calls;
	}

	for (unsigned int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;
		vector<pair<string, string>> indirectCalls = getCallsStar(right, st);
		for (unsigned int j=0; j<indirectCalls.size(); j++) {
			string indirectRight = indirectCalls[j].second;
			pair<string, string> callsStar ( left, indirectRight );
			results.insert(callsStar);
		}
		results.insert(calls[i]);
	}
	return vector<pair<string, string>>(results.begin(), results.end());
}

// Method for Calls* abstraction with both procedures known
// recursively calls the method for getting Calls* with the calling procedure known
// for every called procedure which will return its transitive called procedure and find
// the called procedure that matches the provided called procedure
vector<pair<string, string>> PKBFacade::getCallsStar(string procName1, string procName2) {
	set<pair<string, string>> results;
	vector<pair<string, string>> calls = getCalls(procName1, procedure);
	if (calls.empty()){
		return calls;
	}

	for (unsigned int i=0; i<calls.size(); i++) {
		string left = calls[i].first;
		string right = calls[i].second;;
		if (right.compare(procName2) == 0) {
			results.insert(calls[i]);
		} else {
			vector<pair<string, string>> indirectCalls = getCallsStar(right, procedure);
			for (unsigned int j=0; j<indirectCalls.size(); j++) {
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

// The mehod used to get Next relationship of 2 unknown argument. 
//This equivalent to getting all Next pair in the program. Returns
//a vector contains pairs with Next relationship.
vector<pair<string, string>> PKBFacade::getNext(SyntType s1, SyntType s2){
	if(s1 == progline){
		s1 = statement;
	}
	if(s2 == progline){
		s2 = statement;
	}
	if(s1 == statement && s2 == statement){
		return cfg.getAllPairs();
	}
	vector<pair<string,string>> results;
	std::vector<Node*> stmtVector = statementTable.getStatements(s1);
	for ( int i = 0; i < stmtVector.size(); i++){
		Node* temp = stmtVector.at(i);
		if(temp->getType() == whileLoop || temp->getType() == ifelse){
			CFGNode* nextCFG1 = temp->getCfgNode()->getNextNode();
			CFGNode* nextCFG2 = temp->getCfgNode()->getExNextNode();
			if(nextCFG1){
				Node* stmt = nextCFG1->getStatement();
				if(stmt->getType() == s2 || (stmt->isStatement() && s2 == statement)){
					pair<string, string> nextPair (temp->getLine(),stmt->getLine());
					results.push_back(nextPair);
				}
			}
			if(nextCFG2){
				Node* stmt = nextCFG2->getStatement();
				if(stmt->getType() == s2 || (stmt->isStatement() && s2 == statement)){
					pair<string, string> nextPair (temp->getLine(),stmt->getLine());
					results.push_back(nextPair);
				}
			}
		}
		else{
			CFGNode* nextCFG = temp->getCfgNode()->getNextNode();
			if(nextCFG){
				Node* stmt = nextCFG->getStatement();
				if(stmt->getType() == s2 || (stmt->isStatement() && s2 == statement)){
					pair<string, string> nextPair (temp->getLine(),stmt->getLine());
					results.push_back(nextPair);
				}
			}
		}
	}
	return results;
}

//The method used to get Next relationship between 1 unknown and 1 known
//argument. The known argument is the right argument, meaning this method is to 
//get the previous statement of the known argument. Return vector contains
//the pair of statements that has the Next relationship required
//Pre: s1 - the unknown argument
//	 sNum2 - the known argument
//Post: results - the vector contains pairs of statements
vector<pair<string, string>> PKBFacade::getNext(SyntType s1, string sNum2) {
	if ( s1 == progline){
		s1 = statement;
	}
	vector<pair<string, string>> results;
	vector<CFGNode*> prevList = statementTable.getStatement(sNum2)->getCfgNode()->getPrevNode();
	for (unsigned int i=0; i<prevList.size(); i++) {
		Node* temp = prevList.at(i)->getStatement();
		if ( temp->getType() == s1 || (temp->isStatement() && s1 == statement)){
			pair<string, string> nextPair (temp->getLine(),sNum2);
			results.push_back(nextPair);
		}
	}
	return results;
}

//The method used to get Next relationship between 1 unknown and 1 known
//argument. The known argument is the left argument, meaning this method is to 
//get the next statement of the known argument. Return vector contains
//the pair of statements that has the Next relationship required
//Pre: sNum1 - the known argument
//	 s2 - the unknown argument
//Post: results - the vector contains pairs of statements
vector<pair<string, string>> PKBFacade::getNext(string sNum1, SyntType s2) {
	if(s2 == progline){
		s2 = statement;
	}
	vector<pair<string, string>> results;
	Node* temp = statementTable.getStatement(sNum1);
	if (temp->getType() == whileLoop || temp->getType() == ifelse) {
		CFGNode* nextCFG1 = temp->getCfgNode()->getNextNode();
		CFGNode* nextCFG2 = temp->getCfgNode()->getExNextNode();
		if (nextCFG1) {
			Node* next = nextCFG1->getStatement();
			if(next->getType() == s2 || (next->isStatement() && s2 == statement)){
				pair<string, string> pair1 (sNum1, next->getLine());
				results.push_back(pair1);
			}
		}

		if (nextCFG2) {
			Node* next = nextCFG2->getStatement();
			if(next->getType() == s2 || (next->isStatement() && s2 == statement)){
				pair<string, string> pair2 (sNum1, next->getLine());
				results.push_back(pair2);
			}
		}
	} else {
		CFGNode* nextCFG = temp->getCfgNode()->getNextNode();
		if (nextCFG) {
			Node* next = nextCFG->getStatement();
			if(next->getType() == s2 || (next->isStatement() && s2 == statement)){
				pair<string, string> pair (sNum1, next->getLine());
				results.push_back(pair);
			}
		}
	}
	
	return results;
}

//The method used to get Next relationship between 2 known argument. The
//method search for all Next of left argument and then compare with right argument.
//If matched, it is the result we need. Return the vector contains the result.
//Pre: sNum1 - the left known argument
//	 sNum2 - the right known argument
//Post: results - the vector contains pairs of statements
vector<pair<string, string>> PKBFacade::getNext(string sNum1, string sNum2) {
	vector<pair<string, string>> results;
	Node* temp = statementTable.getStatement(sNum1);
	if (temp->getType() == whileLoop || temp->getType() == ifelse) {
		Node* next1 = temp->getCfgNode()->getNextNode()->getStatement();
		Node* next2 = temp->getCfgNode()->getExNextNode()->getStatement();
		if (next1->getLine() == sNum2) {
			pair<string, string> pair1 (sNum1, next1->getLine());
			results.push_back(pair1);
		}
		if (next2->getLine() == sNum2) {
			pair<string, string> pair2 (sNum1, next2->getLine());
			results.push_back(pair2);
		}
	} else {
		Node* next = temp->getCfgNode()->getNextNode()->getStatement();
		if (next->getLine() == sNum2) {
			pair<string, string> pair (sNum1, next->getLine());
			results.push_back(pair);
		}
	}
	return results;
}

//The method used to get Next* relationship of 2 unknwons argument.
//The method recursively call getNextStar with the left argument to be every
//statement of the program and every pair of statements will be included in the
//result. Return the vector contains all results
//Pre: s1 - the left unknown argument
//	 s2 - the right unknown argument
//Post: results - the vector contains pairs of statements
vector<pair<string, string>> PKBFacade::getNextStar(SyntType s1, SyntType s2) {
	if(s1 == progline){
		s1 = statement;
	}
	if(s2 == progline){
		s2 = statement;
	}
	vector<pair<string, string>> results;
	std::vector<Node*> stmtVector = statementTable.getStatements(s1);
	for (int i = 0 ; i < stmtVector.size() ; i++) {
		Node* stmt = stmtVector.at(i);
		vector<pair<string, string>> temp = getNextStar(stmt->getLine(),s2);
		results.insert(results.end(),temp.begin(),temp.end());
	}

	return results;
}

//The method used to get Next* relationship between 1 unknown and 1 known
//argument. The known argument is the right argument, meaning this method is to 
//get all statement before the known argument that can reach it. Return vector contains
//all results
//Pre: s1 - the unknown argument
//	 sNum2 - the known argument
//Post: results - the vector contains pairs of statements
vector<pair<string, string>> PKBFacade::getNextStar(SyntType s1, string num2) {
	if(s1 == progline){
		s1 = statement;
	}
	vector<pair<string, string>> results;
	Node* tempNode = statementTable.getStatement(num2);
	if (tempNode->getType() == whileLoop) {
		if(s1 == statement || s1 == whileLoop){
			pair<string, string> nextPair (num2,num2);
			results.push_back(nextPair);
		}
		vector<pair<string, string>> children = getParentsStar(num2,s1);
		for (unsigned int j=0; j<children.size(); j++) {
			pair<string, string> nextPair (children[j].second,num2);
			results.push_back(nextPair);
		}
	}

	vector<CFGNode*> prevNodes = tempNode->getCfgNode()->getPrevNode();
	for (unsigned int j = 0; j < prevNodes.size(); j++) {
		Node* pNode = prevNodes[j]->getStatement();
		if( pNode->getType() == s1 || (pNode->isStatement() && s1 == statement)){
			pair<string, string> prevPair (pNode->getLine(),num2);
			results.push_back(prevPair);
		}
		if (atoi(pNode->getLine().c_str()) < atoi(num2.c_str())) {
			vector<pair<string, string>> prevResults = getNextStar(s1,pNode->getLine());
			for (unsigned int i = 0; i < prevResults.size(); i++) {
				pair<string, string> prevPair (prevResults[i].first,num2);
				results.push_back(prevPair);
			}
		}
	}
	set<pair<string, string>> s (results.begin(),results.end());
	results.assign(s.begin(),s.end());
	return results;
}

//The method used to get Next relationship between 1 unknown and 1 known
//argument. The known argument is the left argument, meaning this method is to 
//get the all statements that the known argument can reach. Return vector contains
//all results
//Pre: sNum1 - the known argument
//	 s2 - the unknown argument
//Post: results - the vector contains pairs of statements
vector<pair<string, string>> PKBFacade::getNextStar(string num1, SyntType s2) {
	if(s2 == progline){
		s2 = statement;
	}
	vector<pair<string, string>> results;
	Node* tempNode = statementTable.getStatement(num1);
	if (tempNode->getType() == whileLoop || tempNode->getType() == ifelse) {
		vector<pair<string, string>> childrens = getParentsStar(tempNode->getLine(),s2);
		for (unsigned int j=0; j<childrens.size(); j++) {
			pair<string, string> nextPair (num1,childrens[j].second);
			results.push_back(nextPair);
		}
		CFGNode* nextNode;
		if (tempNode->getType() == whileLoop) {
			if( s2 == statement || s2 == whileLoop){
				pair<string, string> nextPair (num1,tempNode->getLine());
				results.push_back(nextPair);
			}
			nextNode = tempNode->getCfgNode()->getNextNode();
		} else {
			nextNode = tempNode->getCfgNode()->getEndLeft()->getNextNode();
		}
		if (nextNode) {
			Node* stmt = nextNode->getStatement();
			if( stmt->getType() == s2 || ( stmt->isStatement() && s2 == statement)){
				pair<string, string> nextPair (num1,stmt->getLine());
				results.push_back(nextPair);
			}
			vector<pair<string,string>> tempResults = getNextStar(stmt->getLine(),s2);
			for (unsigned int i = 0; i<tempResults.size(); i++) {
				pair<string, string> nextPair (num1,tempResults[i].second);
				results.push_back(nextPair);
			}
		}
	} else {
		CFGNode* nextNode = tempNode->getCfgNode()->getNextNode();
		if (nextNode) {
			Node* stmt = nextNode->getStatement();
			if( stmt->getType() == s2 || ( stmt->isStatement() && s2 == statement)){
				pair<string, string> nextPair (num1,stmt->getLine());
				results.push_back(nextPair);
			}
			vector<pair<string,string>> tempResults = getNextStar(stmt->getLine(),s2);
			for (unsigned int i=0; i<tempResults.size(); i++) {
				pair<string, string> nextPair (num1,tempResults[i].second);
				results.push_back(nextPair);
			}
		}
	}

	set<pair<string, string>> s (results.begin(),results.end());
	results.assign(s.begin(),s.end());
	return results;
}

//The method used to get Next* relationship between 2 knowns argument.
//This is to check if the left argument can reach the right argument. Return 
//the vector that contains the pair if the pair exist.
//Pre: sNum1 - the left known argument
//	 sNum2 - the right known argument
//Post: results - the vector contains pairs of statements
vector<pair<string, string>> PKBFacade::getNextStar(string num1, string num2) {
	vector<pair<string, string>> results;
	vector<pair<string, string>> nextStar = getNextStar(num1,statement);
	for (unsigned int i=0; i<nextStar.size(); i++) {
		if(nextStar[i].second == num2) {
			pair<string, string> nextPair (num1,num2);
			results.push_back(nextPair);
			break;
		}
	}
	return results;
}

// Method for Affects with 2 unknowns
// Retrieves all the assignment statements and calls
// Affects for the known left argument using every assignment statement
vector<pair<string, string>> PKBFacade::getAffects(SyntType st1, SyntType st2) {
	vector<pair<string, string>> results;
	vector<Node*> assignments = statementTable.getStatements(assignment);
	for (unsigned int i=0; i<assignments.size(); i++) {
		vector<pair<string, string>> affects = getAffects(assignments[i]->getLine(), assignment);
		results.insert(results.end(), affects.begin(), affects.end());
	}
	return results;
}

// Method for Affects with the right assignment known
// checks to see if the provided statement number is actually an assignment
// and for every variable that assignment uses
// call reverseAffectsSearch which will recursively search all possible
// execution paths from that assignment backwards for Affects relations
vector<pair<string, string>> PKBFacade::getAffects(SyntType st, string assign) {
	set<pair<string, string>> results;
	Node* assignnode = statementTable.getStatement(assign);
	if (assignnode->getType() != assignment) {
		return vector<pair<string, string>>();
	}
	vector<pair<string, string>> uses = getUses(assign, variable);
	for (unsigned int i=0; i<uses.size(); i++) {
		if (modifiesTable.isModified(assignnode->getRoot()->getValue(), uses[i].second)) {
			set<pair<string, Node*>> searched;
			reverseAffectsSearch(&results, &searched, assignnode, assign, uses[i].second, false);
		}
	}
	return vector<pair<string, string>> (results.begin(), results.end());
}

// Method for Affects with the left assignment known
// checks to see if the provided statement number is actually an assignment
// checks to see if the variable it modifies is actually used in the same procedure
// call forwardAffectsSearch which will recursively search all possible
// execution paths from that assignment statement forwards
vector<pair<string, string>> PKBFacade::getAffects(string assign, SyntType st) {
	set<pair<string, string>> results;
	Node* assignnode = statementTable.getStatement(assign);
	if (assignnode->getType() != assignment) {
		return vector<pair<string, string>>();
	}
	string varName = assignnode->getLeftChild()->getValue();
	if (!usesTable.isUsed(assignnode->getRoot()->getValue(), varName)) {
		return vector<pair<string, string>>();
	}
	set<pair<string, Node*>> searched;
	forwardAffectsSearch(&results, &searched, assignnode, assign, varName, "", false);
	return vector<pair<string, string>> (results.begin(), results.end()); 
}

// Method for Affects with both assignments known
// checks to see if the provided statement numbers is actually an assignment
// checks to see if both assignments are in the same procedure
// checks if the second assignment actually uses the variable the first 
// assignment modifies
// call forwardAffectsSearch which will recursively search all possible
// execution paths from that assignment statement forwards until it finds 
// the matching assignment 2
vector<pair<string, string>> PKBFacade::getAffects(string assign1, string assign2) {
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
	if (!usesTable.isUsed(assign2node->getLine(), varName)) {
		return vector<pair<string, string>>();
	}
	set<pair<string, Node*>> searched;
	forwardAffectsSearch(&results, &searched, assign1node, assign1, varName, assign2, false);
	return vector<pair<string, string>> (results.begin(), results.end());
}

// Method for Affects* with 2 unknowns
// Retrieves all the assignment statements and calls
// Affects* for the known left argument using every assignment statement
vector<pair<string, string>> PKBFacade::getAffectsStar(SyntType st1, SyntType st2) {
	vector<pair<string, string>> results;
	vector<Node*> assignments = statementTable.getStatements(assignment);
	for (unsigned int i=0; i<assignments.size(); i++) {
		vector<pair<string, string>> affects = getAffectsStar(assignments[i]->getLine(), assignment);
		results.insert(results.end(), affects.begin(), affects.end());
	}
	return results;
}

// Method for Affects* with the right assignment known
// checks to see if the provided statement number is actually an assignment
// and for every variable that assignment uses
// call reverseAffectsSearch which will recursively search all possible
// execution paths from that assignment backwards for Affects relations
// and flag the search as transitive
vector<pair<string, string>> PKBFacade::getAffectsStar(SyntType st, string assign) {
	set<pair<string, string>> results;
	Node* assignnode = statementTable.getStatement(assign);
	if (assignnode->getType() != assignment) {
		return vector<pair<string, string>>();
	}
	vector<pair<string, string>> uses = getUses(assign, variable);
	for (unsigned int i=0; i<uses.size(); i++) {
		if(modifiesTable.isModified(assignnode->getRoot()->getValue(), uses[i].second)) {
			set<pair<string, Node*>> searched;
			reverseAffectsSearch(&results, &searched, assignnode, assign, uses[i].second, true);
		}
	}
	return vector<pair<string, string>> (results.begin(), results.end());
}

// Method for Affects* with the left assignment known
// checks to see if the provided statement number is actually an assignment
// checks to see if the variable it modifies is actually used in the same procedure
// call forwardAffectsSearch which will recursively search all possible
// execution paths from that assignment statement forwards
// and flag the search as transitive
vector<pair<string, string>> PKBFacade::getAffectsStar(string assign, SyntType st) {
	set<pair<string, string>> results;
	Node* assignnode = statementTable.getStatement(assign);
	if (assignnode->getType() != assignment) {
		return vector<pair<string, string>>();
	}
	string varName = assignnode->getLeftChild()->getValue();
	if (!usesTable.isUsed(assignnode->getRoot()->getValue(), varName)) {
		return vector<pair<string, string>>();
	}
	set<pair<string, Node*>> searched;
	forwardAffectsSearch(&results, &searched, assignnode, assign, varName, "", true);
	return vector<pair<string, string>> (results.begin(), results.end());
}

// Method for Affects* with both assignments known
// checks to see if the provided statement numbers is actually an assignment
// checks to see if both assignments are in the same procedure
// checks if the procedure uses the variable the left assignment modifies
// call forwardAffectsSearch which will recursively search all possible
// execution paths from that assignment statement forwards until it finds 
// the matching assignment 2 with transitive flag enabled
vector<pair<string, string>> PKBFacade::getAffectsStar(string assign1, string assign2) {
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

//The method used get a statement that has a specific pattern. It takes in the type of 
//statement we are searching for,a left string in the query and the right string of the query. 
//Left represent control variable if type is "while" of "if". It represents the modified statement
//if the type is "assignment". The right string represents the pattern of the right side of the
//assignment if the type is "assignment".
//Pre: type - type of statement
//	 left - the left string
//	 right - the right string
//Post: results - the vector contains pairs of statements
vector<pair<string, string>> PKBFacade::searchWithPattern(SyntType type,string left,string right) {
	vector<pair<string, string>> results;
	string firstChar = right.substr(0,1);
	//if the pattern is for a while loop
	if (type == whileLoop){
		vector<Node*> whileList = getNodes(whileLoop);
		if (left == "_") {
			for (unsigned int i=0; i<whileList.size(); i++) {
				pair<string, string> stmtPair (whileList[i]->getLine(), whileList[i]->getLeftChild()->getValue());
				results.push_back(stmtPair);
			}
		} else {
			for (unsigned int i = 0 ; i < whileList.size() ; i++){
				if (whileList.at(i)->getLeftChild()->getValue().compare(left) == 0){
					pair<string, string> stmtPair (whileList[i]->getLine(), whileList[i]->getLeftChild()->getValue());
					results.push_back(stmtPair);
				}
			}
		}
	} else if (type == ifelse){
		vector<Node*> ifList = getNodes(ifelse);
		if (left == "_"){
			for (unsigned int i=0; i<ifList.size(); i++) {
				pair<string, string> stmtPair (ifList[i]->getLine(), ifList[i]->getLeftChild()->getValue());
				results.push_back(stmtPair);
			}
		}
		else {
			for (unsigned int i = 0 ; i < ifList.size() ; i++) {
				if (ifList.at(i)->getLeftChild()->getValue().compare(left) == 0) {
					pair<string, string> stmtPair (ifList[i]->getLine(), ifList[i]->getLeftChild()->getValue());
					results.push_back(stmtPair);
				}
			}
		}

	//if the pattern is for an assignment
	} else {
		//if the right side was "_"
		if (right == "_") {
			if (left == "_") {
				vector<Node*> nodes = getNodes(assignment);
				for (unsigned int i=0; i<nodes.size(); i++) {
					pair<string, string> stmtPair (nodes[i]->getLine(), nodes[i]->getLeftChild()->getValue());
					results.push_back(stmtPair);
				}
			} else {
				results = getModifies(assignment, left);
			}
		}
		//if the right side is not an expression (eg. variable or constant)
		if (!isExpression(right)) {
			vector<Node*> exprList;
			if (firstChar == "_" ) {
				exprList = getExpressions(right.substr(1,right.length()-2));
				for (unsigned int i = 0 ; i < exprList.size() ; i++ ) {
					Node* parentNode = exprList.at(i)->getExpParent();
					if (left == "_") {
						pair<string, string> stmtPair (parentNode->getLine(), parentNode->getLeftChild()->getValue());
						results.push_back(stmtPair);
					} else {
						if (parentNode->getLeftChild()->getValue() == left) {
							pair<string, string> stmtPair (parentNode->getLine(), parentNode->getLeftChild()->getValue());
							results.push_back(stmtPair);
						}
					}
				}
				return results;
			}
			
			exprList = getRootExpressions(right);
			for (unsigned int i = 0 ; i < exprList.size() ; i++ ) {
				Node* parentNode = exprList.at(i)->getParent();
				if ( left == "_" ) {
					pair<string, string> stmtPair (parentNode->getLine(), parentNode->getLeftChild()->getValue());
					results.push_back(stmtPair);
				} else {
					if (parentNode->getLeftChild()->getValue() == left) {
						pair<string, string> stmtPair (parentNode->getLine(), parentNode->getLeftChild()->getValue());
						results.push_back(stmtPair);
					}
				}
			}
			return results;
		}

		//if the right was not "_" and it is an expression
		Node* tNode;
		vector<Node*> exprList;
		if (firstChar != "_") {
			tNode = et->exptreeSetupSON(et->expressionConverter(right));
			exprList = getRootExpressions(tNode->getValue());
		} else {
			tNode = et->exptreeSetupSON(et->expressionConverter(right.substr(1,right.length()-2)));
			exprList = getExpressions(tNode->getValue());
		}

		for (unsigned int i = 0 ; i < exprList.size() ; i++) {
			Node* tNode2 = exprList.at(i);
			int compareResult = compare(tNode2,tNode);
			if (compareResult != 0) {
				Node* parentNode = tNode2->getParent();
				if (firstChar != "_"){
					if (parentNode->getType() == assignment) {
						if (left == "_") {
							pair<string, string> stmtPair (parentNode->getLine(), parentNode->getLeftChild()->getValue());
							results.push_back(stmtPair);
						} else {
							if (parentNode->getLeftChild()->getValue() == left) {
								pair<string, string> stmtPair (parentNode->getLine(), parentNode->getLeftChild()->getValue());
								results.push_back(stmtPair);
							}
						}
					}
				} else {
					parentNode = tNode2->getExpParent();
					if (left == "_") {
						pair<string, string> stmtPair (parentNode->getLine(), parentNode->getLeftChild()->getValue());
						results.push_back(stmtPair);
					}
					else{
						if(parentNode->getLeftChild()->getValue() == left) {
							pair<string, string> stmtPair (parentNode->getLine(), parentNode->getLeftChild()->getValue());
							results.push_back(stmtPair);
						}
					}
				}
			}
		}
	}
	return results;
}

// compare 2 expression trees
int PKBFacade::compare(Node* p,Node* q){
	if (p == NULL && q == NULL) {
		return 1;
	} else if (p == NULL || q == NULL) {
		return 0;
	} else if (p->getValue() == q->getValue()) {
		int s1=compare(p->getLeftChild(),q->getLeftChild());
		int s2=compare(p->getRightChild(),q->getRightChild());
		return s1*s2;
	} else {
		return 0;
	}
}

// checks if a string contains an expression
bool PKBFacade::isExpression(string s){
	if (s.find("+") != string::npos || s.find("*") != string::npos || s.find("-") != string::npos){
		return true;
	}

	return false;
}

// method that handles passes the inheritance of Modifies and Uses when parsing a statements in a procedure
// to pass the inheritance to other procedures and parents of the statement that calls this statements procedure
// find the procedures that calls this procedure
// finds all the statements that calls this procedure
// calls the methods to handle passing Modifies and Uses for the calling statements and calling procedures
// this method is meant for processing called procedures that were called before they were delcared during parsing
void PKBFacade::handleInheritance(Node* procNode, Node* node, Node* modifiedBy, Node* usedBy) {
	if (procNode != nullptr && callsTable.isCalled(procNode->getValue())) {
		vector<pair<string, string>> calls = getCallsStar(procedure, procNode->getValue());
		for (unsigned int i=0; i<calls.size(); i++) {
			Node* callingProc = procedureTable.getProcedure(calls[i].first);
			vector<string> callingStatements = callsTable.getCallingStatement(calls[i].first);
			vector<string> callingStatements2 = callsTable.getCallingStatement(procNode->getValue());
			callingStatements.insert(callingStatements.end(), callingStatements2.begin(), callingStatements2.end());
			if (modifiedBy != nullptr) {
				handleModifiesInheritance(callingStatements, callingProc, node);
			} else if (usedBy != nullptr) {
				handleUsesInheritance(callingStatements, callingProc, node);
			}
		}
	}
}

// method to handle modifies inheritance
// adds the modifies abstraction for the calling procedures and calling statements
// and adds the modifies abstraction for parents of those calling statements
void PKBFacade::handleModifiesInheritance(vector<string> callingStatements, Node* callingProc, Node* node) {
	modifiesTable.addModifies(callingProc, node->getValue());
	for (unsigned int j=0; j<callingStatements.size(); j++) {
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
}

// method to handle uses inheritance
// adds the uses abstraction for the calling procedures and calling statements
// and adds the uses abstraction for parents of those calling statements
void PKBFacade::handleUsesInheritance(vector<string> callingStatements, Node* callingProc, Node* node) {
	usesTable.addUses(callingProc, node->getValue());
	for (unsigned int j=0; j<callingStatements.size(); j++) {
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

// method to handle the passing of modifies and uses from a procedure thats being called
// to the calling statement and the procedure the calling statement is in, as well as its parents
// gets the modifies and uses abstraction for the procedure being called
// gets the procedures that call this procedure as well 
// gets the statements that call this procedure as well
// calls the methods to handle the passing of modifies and uses in calls
// this method is meant for processing called procedures that were declared before being called during parsing
void PKBFacade::handleCalls(Node* callingProc, Node* node, Node* parent) {
	callsTable.addCalls(callingProc, node);
	Node* targetProc = procedureTable.getProcedure(node->getValue());
	if (targetProc != nullptr ) {
		vector<pair<string, string>> calls = getCallsStar(targetProc->getValue(), procedure);;
		vector<pair<string, string>> callsInherit = getCallsStar(procedure, targetProc->getValue());
		calls.insert(calls.end(), callsInherit.begin(), callsInherit.end());
		for (unsigned int i=0; i<calls.size(); i++) {
			string calledProc = calls[i].second;
			vector<pair<string, string>> modifies = getModifies(calledProc, variable);
			vector<pair<string, string>> uses = getUses(calledProc, variable);
			vector<string> callingStatements = callsTable.getCallingStatement(calls[i].first);
			vector<string> callingStatements2 = callsTable.getCallingStatement(node->getValue());
			callingStatements.insert(callingStatements.end(), callingStatements2.begin(), callingStatements2.end());
			Node* proc = procedureTable.getProcedure(calls[i].first);
			handleModifiesCalls(modifies, callingStatements, parent, proc, node);
			handleUsesCalls(uses, callingStatements, parent, proc, node);
		}
	}
}

// method to handle modifies passing
// for every modifies abstraction in the called procedure
// add it the calling statement, transitive calling procedure and transitive calling statements
// as well as the parents of every calling statement
void PKBFacade::handleModifiesCalls(vector<pair<string, string>> modifies, vector<string> callingStatements, Node* parent, Node* proc, Node* node) {
	for (unsigned int j=0; j<modifies.size(); j++) {
		modifiesTable.addModifies(proc, modifies[j].second);
		for (unsigned int k=0; k<callingStatements.size(); k++) {
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
}

// method to handle uses passing
// for every modifies abstraction in the called procedure
// add it the calling statement, transitive calling procedure and transitive calling statements
// as well as the parents of every calling statement
void PKBFacade::handleUsesCalls(vector<pair<string, string>> uses, vector<string> callingStatements, Node* parent, Node* proc, Node* node) {
	for (unsigned int j=0; j<uses.size(); j++) {
		usesTable.addUses(proc, uses[j].second);
		for (unsigned int k=0; k<callingStatements.size(); k++) {
			Node* callsNode = statementTable.getStatement(callingStatements[k]);
			usesTable.addUses(callsNode, uses[j].second);
			Node* grandParent = parent;
			if (callsNode != node) {
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

// method to handle the adding of modifies abstraction for the node that has been created
// adds the modifies abstraction for the node, for the procedure, and all parents of the node
void PKBFacade::handleModifiedBy(Node* node, Node* modifiedBy, Node* procedure, Node* parent) {
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

// method to handle the adding of uses abstraction for the node that has been created
// adds the uses abstraction for the node, for the procedure, and all parents of the node
void PKBFacade::handleUsedBy(Node* node, Node* usedBy, Node* procedure, Node* parent) {
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

// method to handle the adding of parents abstraction
void PKBFacade::handleParent(Node* child, Node* parent) {
	parentsTable.addChild(parent, child);
}

// find the followee of a statement by directly accessing the
// index and inverted index of its statement list node
Node* PKBFacade::getFollowee(Node* statement) {
	Node* stmtLstNode = statement->getParent();
	unsigned int index = stmtLstNode->getIndexLst()[statement];
	if (index < (stmtLstNode->getIndexLst().size())) {
		return stmtLstNode->getStmtLst()[index + LIST_INDEX_OFFSET];
	}
	return nullptr;
}

// find the follower of a statement by directly accessing the
// index and inverted index of its statement list node
Node* PKBFacade::getFollowedBy(Node* statement) {
	Node* stmtLstNode = statement->getParent();
	int index = stmtLstNode->getIndexLst()[statement];
	if(index > 0) {
		return stmtLstNode->getStmtLst()[index - LIST_INDEX_OFFSET];
	}
	return nullptr;
}

// method for searching execution paths from a given starting point and variable for affects abstraction
// Performs a depth first search with branching and joining
// A target assignment statement is optional. If provided search will end once that 
// target is reached, and will only append the Affects* abstraction for the provided start and end
// if not, all nodes found that meet the requires during execution path search will be appended to the result vector
// 
// For every node that can be reached next from the current node
// check to see if that node has been searched before for the target variable
// if it has not been searched before, append an entry to the searched vector
//     if the statement is an assignment statement and uses the target variable
//         if no target assignment is provided, or if the target assignment matches
//             append to result vector
//         if no target assignment is provided, or if the result vector is empty, and statement doesnt modify this variable
//             recursively call this method with the current node
//         if the transitive flag is enabled, and no target assignment is provided or result vector is empty
//             recursively call this method with the current node and set target vector to the variable this node modifies
// if statement is assignment but doesnt use the target variable
// or statement is a call but doesnt modify target variable
// or statement isnt a call or assignment
// recursively call this method with the current node
void PKBFacade::forwardAffectsSearch(set<pair<string, string>>* results, set<pair<string, Node*>>* searched, Node* node, string origin, string varName, string stop, bool transitive) {
	vector<pair<string, string>> next = getNext(node->getLine(), progline); 
	for (unsigned int i=0; i<next.size(); i++) { 
		Node* stmt = statementTable.getStatement(next[i].second);
		if (searched->find(make_pair(varName, stmt)) == searched->end()) { 
			searched->insert(make_pair(varName, stmt));
			if ((stmt->getType() == assignment && usesTable.isUsed(stmt->getLine(), varName))) {
				if (stop == NO_STOP || stop == stmt->getLine()) {
					results->insert(make_pair(origin, stmt->getLine()));
				}
				if ((stop == NO_STOP || results->size() == 0) && !modifiesTable.isModified(stmt->getLine(), varName)) {
					forwardAffectsSearch(results, searched, stmt, origin, varName, stop, transitive);
				}
				if (transitive && (stop == NO_STOP || results->size() == 0)) { 
					forwardAffectsSearch(results, searched, stmt, origin, stmt->getLeftChild()->getValue(), stop, transitive);
				}
			} else if ((stmt->getType() == assignment && stmt->getLeftChild()->getValue() != varName)
					|| (stmt->getType() == call && !modifiesTable.isModified(stmt->getLine(), varName))
					|| (stmt->getType() != call && stmt->getType() != assignment)){
				forwardAffectsSearch(results, searched, stmt, origin, varName, stop, transitive);
			}
		}
	}
}

// method for searching execution paths from a given starting point and variable for affects abstraction
// Performs a depth first search with branching and joining but in reverse execution path
// all nodes found that meet the requires during execution path search will be appended to the result vector
// 
// For every node that can reach the current node
// check to see if that node has been searched before for the target variable
// if it has not been searched before, append an entry to the searched vector
//     if the statement is an assignment statement and modifies the target variable
//         append to result vector
//         if the transitive flag is enabled
//             for every variable that the statement uses, recursively call this method for the 
//             that variable and the current statement
// if statement is assignment but doesnt modify the target variable
// or statement is a call but doesnt modify target variable
// or statement isnt a call or assignment
// recursively call this method with the current node
void PKBFacade::reverseAffectsSearch(set<pair<string, string>>* results, set<pair<string, Node*>>* searched, Node* node, string origin, string varName, bool transitive) {
	vector<pair<string, string>> prev = getNext(progline, node->getLine());
	for (unsigned int i=0; i<prev.size(); i++) {
		Node* stmt = statementTable.getStatement(prev[i].first);
		if (searched->find(make_pair(varName, stmt)) == searched->end()) {
			searched->insert(make_pair(varName, stmt));
			if ((stmt->getType() == assignment && modifiesTable.isModified(stmt->getLine(), varName))) {
				results->insert(make_pair(stmt->getLine(), origin));
				if (transitive) {
					vector<pair<string, string>> uses = getUses(stmt->getLine(), variable);
					for (unsigned int j=0; j<uses.size(); j++) {
						reverseAffectsSearch(results, searched, stmt, origin, uses[j].second, transitive);
					}
				}
			} else if ((stmt->getType() == assignment && !modifiesTable.isModified(stmt->getLine(), varName))
					|| (stmt->getType() == call && !modifiesTable.isModified(stmt->getLine(), varName))
					|| (stmt->getType() != call && stmt->getType() != assignment)) {
				reverseAffectsSearch(results, searched, stmt, origin, varName, transitive);
			}
		}
	}
}

// method to get all expression nodes of a particular expression
// used for pattern matching speed up
vector<Node*> PKBFacade::getExpressions(string expr) {
	return expressionTable.getExpressions(expr);
}

// method to get expression nodes of a particular expression
// if and only if they are the direct child of the assignment statement
// used for pattern matching speed up
vector<Node*> PKBFacade::getRootExpressions(string expr) {
	return expressionTable.getRootExpressions(expr);
}