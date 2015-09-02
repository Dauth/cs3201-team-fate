#include "ProcedureTable.h"

ProcedureTable::ProcedureTable () {

}

AST ProcedureTable::getProcedure(std::string procedureName) {
	return table[procedureName];
}

void ProcedureTable::addProcedure(std::string procedureName, AST procedureAST) {
	table[procedureName] = procedureAST;
}