#include "stdafx.h"
#include "VariableTable.h"

VariableTable::VariableTable () {

}

Variable* VariableTable::getOrCreateVariable(std::string varName) {
	if ( table.find(varName) == table.end() ) {
		Variable* var = new Variable(varName);
		table[varName] = var;
		return var;
	} else {
		return table[varName];
	}
}

std::vector<Node*> VariableTable::getVariableNodes() {
	std::vector<Node*> nodes;
	for(std::map<std::string, Variable*>::iterator it = table.begin(); it != table.end(); ++it) {
		Variable* var = it->second;
		std::vector<Node*> varNodes = var->getNodes();
		nodes.insert(nodes.end(), varNodes.begin(), varNodes.end());
	}
	return nodes;
}

int VariableTable::getVariableCount() {
	return table.size();
}

std::vector<Node*> VariableTable::getModifiedBy(std::string varName) {
	if ( table.find(varName) == table.end() ) {
		return std::vector<Node*>();
	}
	return table[varName]->getModifiedBy();
}

std::vector<Node*> VariableTable::getUsedBy(std::string varName) {
	if ( table.find(varName) == table.end() ) {
		return std::vector<Node*>();
	}
	return table[varName]->getUsedBy();
}

void VariableTable::addModifiedBy(std::string varName, Node* node) {
	table[varName]->addModifiedBy(node);
}

void VariableTable::addUsedBy(std::string varName, Node* node) {
	table[varName]->addUsedBy(node);
}

void VariableTable::addNode(std::string varName, Node* node) {
	table[varName]->addNode(node);
}

void VariableTable::debug() {
	std::cout << table.size() << "\n";
}