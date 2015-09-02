#include "ProcedureTable.h"

ProcedureTable::ProcedureTable () {

}

AST* ProcedureTable::getOrCreateProcedure(std::string procedureName) {
	if ( table.find(procedureName) == table.end() ) {
		Node node(procedure, procedureName) ;
		AST ast(node);
		table[procedureName] = ast;
	}
	return &table[procedureName];
}
