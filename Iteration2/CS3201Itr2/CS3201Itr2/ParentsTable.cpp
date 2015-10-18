#include "stdafx.h"
#include "ParentsTable.h"

using namespace std;

ParentsTable::ParentsTable () {
	leftTypeKeyTable[whileLoop] = set<pair<string, string>>();
	leftTypeKeyTable[ifelse] = set<pair<string, string>>();
	leftTypeKeyTable[statement] = set<pair<string, string>>();

	rightTypeKeyTable[whileLoop] = set<pair<string, string>>();
	rightTypeKeyTable[ifelse] = set<pair<string, string>>();
	rightTypeKeyTable[assignment] = set<pair<string, string>>();
	//rightTypeKeyTable[call] = set<pair<string, string>>();
	rightTypeKeyTable[statement] = set<pair<string, string>>();
}

vector<pair<string, string>> ParentsTable::getByLeftKey(string ident) {
	if (leftKeyTable.find(ident) == leftKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = leftKeyTable[ident];
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> ParentsTable::getByLeftKey(SyntType st) {
	if (leftTypeKeyTable.find(st) == leftTypeKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = leftTypeKeyTable[st];
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> ParentsTable::getByRightKey(string ident) {
	if (rightKeyTable.find(ident) == rightKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = rightKeyTable[ident];
	return vector<pair<string, string>> (results.begin(), results.end());
}

vector<pair<string, string>> ParentsTable::getByRightKey(SyntType st) {
	if (rightTypeKeyTable.find(st) == rightTypeKeyTable.end()) {
		return vector<pair<string, string>>();
	}
	set<pair<string, string>> results = rightTypeKeyTable[st];
	return vector<pair<string, string>> (results.begin(), results.end());
}


void ParentsTable::addChild(Node* nodeLeft, Node* nodeRight) {
	string left = nodeLeft->getLine();
	string right = nodeRight->getLine();
	pair<string, string> modifies ( left, right );

	if ( leftKeyTable.find(left) == leftKeyTable.end() ) {
		 set<pair<string, string>> nodes;
		leftKeyTable[left] = nodes;
	}
	if ( rightKeyTable.find(right) == rightKeyTable.end() ) {
		 set<pair<string, string>> nodes;
		rightKeyTable[right] = nodes;
	}

	leftKeyTable[left].insert(modifies);
	rightKeyTable[right].insert(modifies);


	SyntType st1 = nodeLeft->getType();
	leftTypeKeyTable[st1].insert(modifies);
	if ( nodeLeft->isStatement() ) {
		leftTypeKeyTable[statement].insert(modifies);
	}

	SyntType st2 = nodeRight->getType();
	rightTypeKeyTable[st2].insert(modifies);
	if ( nodeRight->isStatement() ) {
		rightTypeKeyTable[statement].insert(modifies);
	}
}