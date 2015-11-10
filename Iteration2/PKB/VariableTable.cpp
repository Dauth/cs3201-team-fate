#include "stdafx.h"
#include "VariableTable.h"

// variable table is used to keep track of the variables in the PKB
// and to enable quick access to Node pointers that are of the same variable name
VariableTable::VariableTable () {

}

// gets the Nodes with the variable name
std::vector<Node*> VariableTable::getNode(std::string varName) {
	if ( table.find(varName) == table.end() ) {
		return std::vector<Node*>();
	}
	return table[varName];
}

// get all variable nodes
std::vector<Node*> VariableTable::getNodes() {
	std::vector<Node*> nodes;
	for(std::unordered_map<std::string, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); ++it) {
		nodes.push_back(it->second.at(0));
	}
	return nodes;
}

// count number of variables
int VariableTable::getVariableCount() {
	return table.size();
}

// number of variables of a certain name
int VariableTable::getVariableCount(std::string varName) {
	if ( table.find(varName) == table.end() ) {
		return 0;
	}
	return table[varName].size();
}

// add a variable
void VariableTable::addNode(Node* node) {
	if ( table.find(node->getValue()) == table.end() ) {
		table[node->getValue()] = std::vector<Node*>();
	}
	table[node->getValue()].push_back(node);
}