#ifndef CALLSTABLE_H
#define CALLSTABLE_H

#include "stdafx.h"
#include "Node.h"

using namespace std;

class CallsTable {

	public:
		CallsTable();

		vector<pair<string, string>> getByLeftKey(string);
		vector<pair<string, string>> getByRightKey(string);
		vector<pair<string, string>> getAll();
		vector<string> getCallingStatement(string);

		bool isCalled(string);
		bool isCalled(string, string);
		void addCalls(Node*, Node*);

	private:

		unordered_map<string, set<pair<string, string>>> leftKeyTable;
		unordered_map<string, set<pair<string, string>>> rightKeyTable;
		unordered_map<string, set<string>> callingStatement;
		set<pair<string, string>> allCalls;
};

#endif