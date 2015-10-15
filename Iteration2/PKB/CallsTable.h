#ifndef CALLSTABLE_H
#define CALLSTABLE_H

#include "stdafx.h"
#include "Node.h"

using namespace std;

class CallsTable {
	unordered_map<string, set<pair<string, string>>> leftKeyTable;
	unordered_map<string, set<pair<string, string>>> rightKeyTable;
	set<pair<string, string>> allCalls;

public:
	CallsTable();

	vector<pair<string, string>> getByLeftKey(string);
	vector<pair<string, string>> getByRightKey(string);
	vector<pair<string, string>> getAll();
	bool isCalled(string);
	void addCalls(Node*, string);
};

#endif