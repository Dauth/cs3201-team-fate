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

void VariableTable::addModifiedBy(std::string varName, Node* pnode) {
	table[varName]->addModifiedBy(pnode);
}

void VariableTable::addUsedBy(std::string varName, Node* pnode) {
	table[varName]->addUsedBy(pnode);
}

void VariableTable::debug() {
	std::cout << table.size() << "\n";
}