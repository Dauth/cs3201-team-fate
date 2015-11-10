#ifndef MODIFIESTABLE_H
#define MODIFIESTABLE_H

#include "stdafx.h"
#include "Node.h"

using namespace std;

class ModifiesTable {
	
	public:

		ModifiesTable();
		vector<pair<string, string>> getByLeftKey(string);
		vector<pair<string, string>> getByLeftKey(SyntType);
		vector<pair<string, string>> getByRightKey(string,SyntType);\

		void addModifies(Node*, string);
		bool isModified(string, string);

	private:

		unordered_map<string, set<pair<string, string>>> leftKeyTable;
		unordered_map<string, unordered_map<SyntType, set<pair<string, string>>>> rightKeyTable;
		unordered_map<SyntType, set<pair<string, string>>> leftTypeKeyTable;
};

#endif