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

std::vector<Node*> VariableTable::getModifiedBy(std::string varName) {
	return table[varName]->getModifiedBy();
}

std::vector<Node*> VariableTable::getUsedBy(std::string varName) {
	return table[varName]->getUsedBy();
}

void VariableTable::addModifiedBy(std::string varName, Node* pnode) {
	table[varName]->addModifiedBy(pnode);
}

void VariableTable::addUsedBy(std::string varName, Node* pnode) {
	table[varName]->addModifiedBy(pnode);
}