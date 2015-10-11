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

void CallsTable::addCalls(Node* nodeLeft, Node* nodeRight) {
	string left = nodeLeft->getValue();
	string right = nodeRight->getValue();
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