#include "VariableTable.h"

VariableTable::VariableTable () {

}

Variable* VariableTable::getOrCreateVariable(std::string varName) {
	if ( table.find(varName) == table.end() ) {
		Variable var(varName);
		table[varName] = var;
		return &var;
	} else {
		return &table[varName];
	}
}

std::vector<Node*> VariableTable::getModifiedBy(std::string varName) {
	Variable &var = table[varName];
	return var.getModifiedBy();
}

std::vector<Node*> VariableTable::getUsedBy(std::string varName) {
	Variable &var = table[varName];
	return var.getUsedBy();
}

void VariableTable::addModifiedBy(std::string varName, Node* pnode) {
	Variable &var = table[varName];
	var.addModifiedBy(pnode);
}

void VariableTable::addUsedBy(std::string varName, Node* pnode) {
	Variable &var = table[varName];
	var.addModifiedBy(pnode);
}