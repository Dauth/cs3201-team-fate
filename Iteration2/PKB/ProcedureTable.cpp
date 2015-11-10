#include "stdafx.h"
#include "ProcedureTable.h"

// procedure table keeps track of procedures in the source program
// and provides quick access to the root nodes of these procedures
// and also the factory method to create procedure nodes
ProcedureTable::ProcedureTable () {

}

// create new procedure
// if the procedure has been created before, return a null pointer
Node* ProcedureTable::createProcedure(std::string procedureName) {
	if ( table.find(procedureName) == table.end() ) {
		Node* node = new Node(procedure, "0", procedureName); // magic numbers!!
		node->setRoot(node);
		table[procedureName] = node;
		return table[procedureName];
	}
	return nullptr;
}

// gets the procedure node based on its name
Node* ProcedureTable::getProcedure(std::string procedureName) {
	if ( table.find(procedureName) == table.end() ) {
		return nullptr;
	}
	return table[procedureName];
}

// gets all procedure nodes
std::vector<Node*> ProcedureTable::getAllProcedures() {
	std::vector<Node*> procedures;
	for(std::unordered_map<std::string, Node*>::iterator it = table.begin(); it != table.end(); ++it) {
		procedures.push_back(it->second);
	}
	return procedures;
}
