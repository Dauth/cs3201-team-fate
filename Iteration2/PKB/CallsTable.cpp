#include "stdafx.h"
#include "CallsTable.h"

using namespace std;

CallsTable::CallsTable () {
}

vector<pair<string, string>> CallsTable::getByLeftKey(string ident) {
	if (leftKeyTable.find(ident) == leftKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = leftKeyTable[ident];
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> CallsTable::getByRightKey(string ident) {
	if (rightKeyTable.find(ident) == rightKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = rightKeyTable[ident];
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> CallsTable::getAll() {
	return vector<pair<string, string>> (allCalls.begin(), allCalls.end());
}

bool CallsTable::isCalled(string procName) {
	if (rightKeyTable.find(procName) == rightKeyTable.end()) {
		return false;
	}
	return true;
}

void CallsTable::addCalls(Node* nodeLeft, string procName) {
	string left = nodeLeft->getValue();
	string right = procName;
	pair<string, string> calls ( left, right );

	if ( leftKeyTable.find(left) == leftKeyTable.end() ) {
		 set<pair<string, string>> nodes;
		leftKeyTable[left] = nodes;
	}
	if ( rightKeyTable.find(right) == rightKeyTable.end() ) {
		 set<pair<string, string>> nodes;
		rightKeyTable[right] = nodes;
	}

	leftKeyTable[left].insert(calls);
	rightKeyTable[right].insert(calls);
}