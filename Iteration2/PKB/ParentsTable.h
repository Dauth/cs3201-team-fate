#ifndef PARENTSTABLE_H
#define PARENTSTABLE_H

#include "stdafx.h"
#include "Node.h"

using namespace std;

class ParentsTable {
	unordered_map<string, set<pair<string, string>>> leftKeyTable;
	unordered_map<string, set<pair<string, string>>> rightKeyTable;
	unordered_map<synt_type, set<pair<string, string>>> leftTypeKeyTable;
	unordered_map<synt_type, set<pair<string, string>>> rightTypeKeyTable;

public:
	ParentsTable();

	vector<pair<string, string>> getByLeftKey(string);
	vector<pair<string, string>> getByLeftKey(synt_type);
	vector<pair<string, string>> getByRightKey(string);
	vector<pair<string, string>> getByRightKey(synt_type);

	void addChild(Node*, Node*);
};

#endif