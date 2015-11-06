#ifndef USESTABLE_H
#define USESTABLE_H

#include "stdafx.h"
#include "Node.h"

using namespace std;

class UsesTable {
	unordered_map<string, set<pair<string, string>>> leftKeyTable;
	unordered_map<string, unordered_map<SyntType, set<pair<string, string>>>> rightKeyTable;
	unordered_map<SyntType, set<pair<string, string>>> leftTypeKeyTable;
	
	
public:
	UsesTable();
	vector<pair<string, string>> getByLeftKey(string);
	vector<pair<string, string>> getByLeftKey(SyntType);
	vector<pair<string, string>> getByRightKey(string, SyntType);
	bool isUsed(string, string);

	void addUses(Node*, string);
};

#endif