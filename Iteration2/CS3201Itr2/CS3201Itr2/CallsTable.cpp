#include "stdafx.h"
#include "CallsTable.h"

using namespace std;

// calls table keeps track of procedures calling another procedure
CallsTable::CallsTable () {
}

// retrieves a call abstraction based on the left argument of
// a calls abstraction as the key in a hashtable
vector<pair<string, string>> CallsTable::getByLeftKey(string ident) {
	if (leftKeyTable.find(ident) == leftKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = leftKeyTable[ident];
	return vector<pair<string, string>> (results.begin(), results.end());
}

// retrieves a call abstraction based on right argument of
// a calls abstraction as the key in a hashtable
vector<pair<string, string>> CallsTable::getByRightKey(string ident) {
	if (rightKeyTable.find(ident) == rightKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = rightKeyTable[ident];
	return vector<pair<string, string>> (results.begin(), results.end());
}

// get all calls abstractions
vector<pair<string, string>> CallsTable::getAll() {
	return vector<pair<string, string>> (allCalls.begin(), allCalls.end());
}

// find the statements that call the provided procedure name
vector<string> CallsTable::getCallingStatement(string procName) {
	if (callingStatement.find(procName) == callingStatement.end()) {
		return vector<string>();
	}
	set<string> callingStatements = callingStatement[procName];
	return vector<string> (callingStatements.begin(), callingStatements.end());
}

// add a calls abstraction 
// adds entries in a hashtable and inverted hashtable
// adds an entry in a calling statement table
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
	if ( callingStatement.find(right) == callingStatement.end() ) {
		set<string> nodes;
		callingStatement[right] = nodes;
	}

	leftKeyTable[left].insert(calls);
	rightKeyTable[right].insert(calls);
	callingStatement[right].insert(nodeRight->getLine());
	allCalls.insert(calls);
}

// checks to see if the provided procedure is called by any procedure
bool CallsTable::isCalled(string procName) {
	if (rightKeyTable.find(procName) == rightKeyTable.end()) {
		return false;
	}
	return true;
}

// checks if the input calls abstraction holds.
bool CallsTable::isCalled(string proc1, string proc2) {
	if (leftKeyTable.find(proc1) == leftKeyTable.end()) {
		return false;
	}
	set<pair<string, string>> calls = leftKeyTable[proc1];
	if (calls.find(make_pair(proc1, proc2)) == calls.end()) {
		return false;
	}
	return true;
}