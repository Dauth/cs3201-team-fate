#include "Node.h"
#include "Variable.h"
#include "stdafx.h"

Node::Node (synt_type st, int l, std::string v) {
	SYNT_TYPE = st;
	value = v;
	l = l;
}

synt_type Node::getType() {
	return SYNT_TYPE;
}

int Node::getLine() {
	return line;
}

std::string Node::getValue() {
	return value;
}

Node* Node::getRoot() {
	return root;
}

Node* Node::getParent() {
	return parent;
}

Node* Node::getLeftChild() {
	return leftChild;
}

Node* Node::getRightChild() {
	return rightChild;
}

std::map<int, Node*> Node::getStmtLst() {
	return stmtLst;
}

std::map<Node*, int> Node::getIndexLst() {
	return indexLst;
}


Variable* Node::getVariable() {
	return var;
}

void Node::setRoot(Node* node) {
	root = node;
}

void Node::setParent(Node* node) {
	parent = node;
}

void Node::setLeftChild(Node* node) {
	leftChild = node;
}

void Node::setRightChild(Node* node) {
	rightChild = node;
}

void Node::addStmt(Node* node) {
	indexLst[node] = stmtLst.size();
	stmtLst[stmtLst.size()] = node;
}

void Node::setVar(Variable* v) {
	var = v;
}
