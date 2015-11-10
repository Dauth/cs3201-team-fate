#include "stdafx.h"
#include "UsesTable.h"

using namespace std;

// keeps track of Uses abstractions
UsesTable::UsesTable () {
}

// retrieves Uses abstractions based on the statement number or procedure name
vector<pair<string, string>> UsesTable::getByLeftKey(string ident) {
	if (leftKeyTable.find(ident) == leftKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = leftKeyTable[ident];
	return vector<pair<string, string>> (results.begin(), results.end());
}

// retrieves Uses abstraction based on the statement type or procedure
vector<pair<string, string>> UsesTable::getByLeftKey(SyntType st) {
	if (leftTypeKeyTable.find(st) == leftTypeKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = leftTypeKeyTable[st];
	return vector<pair<string, string>> (results.begin(), results.end());
}

// retrieves Uses abstraction based on variable name and statement type or procedure
vector<pair<string, string>> UsesTable::getByRightKey(string ident, SyntType st) {
	if (rightKeyTable.find(ident) == rightKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = rightKeyTable[ident][st];
	return vector<pair<string, string>> (results.begin(), results.end());
}

// adds a uses abstraction
// adds an entry based on statement type or procedure
// adds an entry based on statement number or procedure name
// adds an entry based on variable name and statement type or procedure
// adds an entry based on variable name and general statement type if statement
void UsesTable::addUses(Node* nodeLeft, string varName) {
	string left = nodeLeft->getLine();
	if (nodeLeft->getType() == procedure) {
		left = nodeLeft->getValue();
	}
	string right = varName;
	pair<string, string> modifies ( left, right );

	if ( leftKeyTable.find(left) == leftKeyTable.end() ) {
		 set<pair<string, string>> nodes;
		leftKeyTable[left] = nodes;
	}
	if ( rightKeyTable.find(right) == rightKeyTable.end() ) {
		unordered_map<SyntType, set<pair<string, string>>> nodes;
		rightKeyTable[right] = nodes;
	}
	leftKeyTable[left].insert(modifies);
	SyntType st = nodeLeft->getType();
	leftTypeKeyTable[st].insert(modifies);
	rightKeyTable[right][st].insert(modifies);
	if ( nodeLeft->isStatement() ) {
		leftTypeKeyTable[statement].insert(modifies);
		rightKeyTable[right][statement].insert(modifies);
	}
}

// checks if the Uses abstraction holds
bool UsesTable::isUsed(string stmt, string varname) {
	if (leftKeyTable.find(stmt) == leftKeyTable.end()) {
		return false;
	}
	set<pair<string, string>> uses = leftKeyTable[stmt];
	if (uses.find(make_pair(stmt, varname)) == uses.end()) {
		return false;
	}
	return true;
}