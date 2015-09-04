#include "Variable.h"
#include "Node.h"
#include "stdafx.h"

Variable::Variable(std::string _name) {
	name = _name;
}

std::string Variable::getName() {
	return name;
}

std::vector<Node*> Variable::getUsedBy() {
	return usedBy;
}

std::vector<Node*> Variable::getModifiedBy() {
	return modifiedBy;
}

void Variable::addUsedBy(Node* pnode) {
	usedBy.push_back(pnode);
}

void Variable::addModifiedBy(Node* pnode) {
	modifiedBy.push_back(pnode);
}
