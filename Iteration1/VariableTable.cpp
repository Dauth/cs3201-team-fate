#include "VariableTable.h"

VariableTable::VariableTable () {

}

Variable* VariableTable::getVariable(std::string varName) {
		return table[varName];
}

std::vector<Node*> VariableTable::getModifiedBy(std::string varName) {
	Variable var = *table[varName];
	return var.getModifiedBy();
}