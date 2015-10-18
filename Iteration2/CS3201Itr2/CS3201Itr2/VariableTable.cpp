#include "stdafx.h"
#include "VariableTable.h"

VariableTable::VariableTable () {

}

std::vector<Node*> VariableTable::getNode(std::string varName) {
	if ( table.find(varName) == table.end() ) {
		return std::vector<Node*>();
	}
	return table[varName];
}

std::vector<Node*> VariableTable::getNodes() {
	std::vector<Node*> nodes;
	for(std::unordered_map<std::string, std::vector<Node*>>::iterator it = table.begin(); it != table.end(); ++it) {
		nodes.push_back(it->second.at(0));
	}
	return nodes;
}

int VariableTable::getVariableCount() {
	return table.size();
}

int VariableTable::getVariableCount(std::string varName) {
	if ( table.find(varName) == table.end() ) {
		return 0;
	}
	return table[varName].size();
}

void VariableTable::addNode(Node* node) {
	if ( table.find(node->getValue()) == table.end() ) {
		table[node->getValue()] = std::vector<Node*>();
	}
	table[node->getValue()].push_back(node);
}