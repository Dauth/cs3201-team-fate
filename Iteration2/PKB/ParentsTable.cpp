#include "stdafx.h"
#include "ParentsTable.h"

using namespace std;

ParentsTable::ParentsTable () {
}

// retrieves a Parents abstraction based on the parent statement number
// and the statement type of the child
vector<pair<string, string>> ParentsTable::getByLeftKey(string ident, SyntType st) {
	if (leftKeyTable.find(ident) == leftKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = leftKeyTable[ident][st];
	return vector<pair<string, string>> (results.begin(), results.end());
}

// retrieves a Parents abstraction based on the Parent statement type
// and the child statement type
vector<pair<string, string>> ParentsTable::getByLeftKey(SyntType st1, SyntType st2) {
	if (leftTypeKeyTable.find(st1) == leftTypeKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = leftTypeKeyTable[st1][st2];
	return vector<pair<string, string>> (results.begin(), results.end());
}

// retrieves a Parents abstraction based on the Child statement number
// and the Parent statement type
vector<pair<string, string>> ParentsTable::getByRightKey(string ident, SyntType st) {
	if (rightKeyTable.find(ident) == rightKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = rightKeyTable[ident][st];
	return vector<pair<string, string>> (results.begin(), results.end());
}

// retrieves a Parents abstraction based on the child statement type 
// and the Parent statement type
vector<pair<string, string>> ParentsTable::getByRightKey(SyntType st1, SyntType st2) {
	if (rightTypeKeyTable.find(st1) == rightTypeKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = rightTypeKeyTable[st1][st2];
	return vector<pair<string, string>> (results.begin(), results.end());
}

// adds a Parents abstraction
// Adds a parent child entry for
// Parent statement number and child statement type 
// Parent statement number and child of statement type
// parent statement type and child statement type
// parent statement type and child of statement type
// parent of statement type and child statement type
// parent of statement type and child of statement type
// 
// and entries of the above but inverted
// allows direct access of any Parent abstraction
void ParentsTable::addChild(Node* nodeLeft, Node* nodeRight) {
	string left = nodeLeft->getLine();
	string right = nodeRight->getLine();
	SyntType leftType = nodeLeft->getType();
	SyntType rightType = nodeRight->getType();
	pair<string, string> parent ( left, right );

	if ( leftKeyTable.find(left) == leftKeyTable.end() ) {
		unordered_map<SyntType, set<pair<string, string>>> nodes;
		leftKeyTable[left] = nodes;
	}
	if ( rightKeyTable.find(right) == rightKeyTable.end() ) {
		unordered_map<SyntType, set<pair<string, string>>> nodes;
		rightKeyTable[right] = nodes;
	}

	leftKeyTable[left][rightType].insert(parent);
	leftKeyTable[left][statement].insert(parent);
	leftTypeKeyTable[leftType][rightType].insert(parent);
	leftTypeKeyTable[leftType][statement].insert(parent);
	leftTypeKeyTable[statement][rightType].insert(parent);
	leftTypeKeyTable[statement][statement].insert(parent);

	rightKeyTable[right][leftType].insert(parent);
	rightKeyTable[right][statement].insert(parent);
	rightTypeKeyTable[rightType][leftType].insert(parent);
	rightTypeKeyTable[rightType][statement].insert(parent);
	rightTypeKeyTable[statement][leftType].insert(parent);
	rightTypeKeyTable[statement][statement].insert(parent);
}

// checks if the Parent abstraction holds
bool ParentsTable::isParent(string stmt, string varname) {
	if (leftKeyTable.find(stmt) == leftKeyTable.end()) {
		return false;
	}
	set<pair<string, string>> parents = leftKeyTable[stmt][statement];
	if (parents.find(make_pair(stmt, varname)) == parents.end()) {
		return false;
	}
	return true;
}