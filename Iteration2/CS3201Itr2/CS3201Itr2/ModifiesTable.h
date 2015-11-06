#ifndef MODIFIESTABLE_H
#define MODIFIESTABLE_H

#include "stdafx.h"
#include "Node.h"

using namespace std;

class ModifiesTable {
	unordered_map<string, set<pair<string, string>>> leftKeyTable;
	unordered_map<string, unordered_map<SyntType, set<pair<string, string>>>> rightKeyTable;
	unordered_map<SyntType, set<pair<string, string>>> leftTypeKeyTable;
	
public:
	ModifiesTable();
	
	vector<pair<string, string>> getByLeftKey(string);
	vector<pair<string, string>> getByLeftKey(SyntType);
	vector<pair<string, string>> getByRightKey(string,SyntType);
	bool isModified(string, string);

	void addModifies(Node*, string);
};

#endif