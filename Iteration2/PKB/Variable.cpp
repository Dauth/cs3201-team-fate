#include "stdafx.h"
#include "Variable.h"
#include "Node.h"

Variable::Variable(std::string _name) {
	name = _name;
}

std::string Variable::getName() {
	return name;
}

std::vector<Node*> Variable::getUsedBy() {
	return std::vector<Node*> (usedBy.begin(), usedBy.end());
}

std::vector<Node*> Variable::getModifiedBy() {
	return std::vector<Node*> (modifiedBy.begin(), modifiedBy.end());
}

std::vector<Node*> Variable::getNodes() {
	return nodes;
}

int Variable::getNodeCount() {
	return nodes.size();
}

void Variable::addUsedBy(Node* node) {
	usedBy.insert(node);
}

void Variable::addModifiedBy(Node* node) {
	modifiedBy.insert(node);
}

void Variable::addNode(Node* node) {
	nodes.push_back(node);
}
