#ifndef MODIFIESTABLE_H
#define MODIFIESTABLE_H

#include "stdafx.h"
#include "Node.h"

using namespace std;

class ModifiesTable {
	unordered_map<string, set<pair<string, string>>> leftKeyTable;
	unordered_map<string, set<pair<string, string>>> rightKeyTable;
	unordered_map<synt_type, set<pair<string, string>>> leftTypeKeyTable;
	
public:
	ModifiesTable();
	
	vector<pair<string, string>> getByLeftKey(string);
	vector<pair<string, string>> getByLeftKey(synt_type);
	vector<pair<string, string>> getByRightKey(string);

	void addModifies(Node*, string);
};

#endif