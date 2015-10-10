#include "stdafx.h"
#include "ProcedureTable.h"

ProcedureTable::ProcedureTable () {

}

Node* ProcedureTable::createProcedure(std::string procedureName) {
	if ( table.find(procedureName) == table.end() ) {
		Node* node = new Node(procedure, "0", procedureName); // magic numbers!!
		node->setRoot(node);
		table[procedureName] = node;
		return table[procedureName];
	}
	return nullptr;
}

Node* ProcedureTable::getProcedure(std::string procedureName) {
	if ( table.find(procedureName) == table.end() ) {
		return nullptr;
	}
	return table[procedureName];
}

std::vector<Node*> ProcedureTable::getAllProcedures() {
	std::vector<Node*> procedures;
	for(std::unordered_map<std::string, Node*>::iterator it = table.begin(); it != table.end(); ++it) {
		procedures.push_back(it->second);
	}
	return procedures;
}
