#include "stdafx.h"
#include "ProcedureTable.h"

ProcedureTable::ProcedureTable () {

}

Node* ProcedureTable::getOrCreateProcedure(std::string procedureName) {
	if ( table.find(procedureName) == table.end() ) {
		Node* node = new Node(procedure, 0, procedureName); // magic numbers!!
		node->setRoot(node);
		table[procedureName] = node;
	}
	return table[procedureName];
}
