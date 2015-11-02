#include "stdafx.h"
#include "UsesTable.h"

using namespace std;

UsesTable::UsesTable () {
	leftTypeKeyTable[whileLoop] = set<pair<string, string>>();
	leftTypeKeyTable[ifelse] = set<pair<string, string>>();
	leftTypeKeyTable[assignment] = set<pair<string, string>>();
	//leftTypeKeyTable[call] = set<pair<string, string>>();
	leftTypeKeyTable[statement] = set<pair<string, string>>();
	leftTypeKeyTable[procedure] = set<pair<string, string>>();
}

vector<pair<string, string>> UsesTable::getByLeftKey(string ident) {
	if (leftKeyTable.find(ident) == leftKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = leftKeyTable[ident];
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> UsesTable::getByLeftKey(SyntType st) {
	if (leftTypeKeyTable.find(st) == leftTypeKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = leftTypeKeyTable[st];
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> UsesTable::getByRightKey(string ident) {
	if (rightKeyTable.find(ident) == rightKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = rightKeyTable[ident];
	return vector<pair<string, string>> (results.begin(), results.end());
}

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
		 set<pair<string, string>> nodes;
		rightKeyTable[right] = nodes;
	}

	leftKeyTable[left].insert(modifies);
	rightKeyTable[right].insert(modifies);

	SyntType st = nodeLeft->getType();
	leftTypeKeyTable[st].insert(modifies);
	if ( nodeLeft->isStatement() ) {
		leftTypeKeyTable[statement].insert(modifies);
	}
}

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