#include "stdafx.h"
#include "Node.h"
#include "CFGNode.h"

Node::Node (SyntType st, std::string l, std::string v) {
	syntType = st;
	value = v;
	line = l;
	leftChild = nullptr;
	rightChild = nullptr;
	thirdChild = nullptr;
	root = nullptr;
	parent = nullptr;
	expParent = nullptr;
}

Node::Node (SyntType st, std::string v) {
	syntType = st;
	value = v;
}

SyntType Node::getType() {
	return syntType;
}

std::string Node::getLine() {
	return line;
}

bool Node::isStatement() {
	if ( syntType == whileLoop || syntType == ifelse || syntType == assignment || syntType == call) {
		return true;
	}
	return false;
}

bool Node::isContainer() {
	if ( syntType == whileLoop || syntType == ifelse ) {
		return true;
	}
	return false;
}

std::string Node::getValue() {
	return value;
}

Node* Node::getRoot() {
	return root;
}

Node* Node::getExpParent() {
	return expParent;
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

Node* Node::getThirdChild() {
	return thirdChild;
}

std::map<int, Node*> Node::getStmtLst() {
	return stmtLst;
}

std::map<Node*, int> Node::getIndexLst() {
	return indexLst;
}

CFGNode* Node::getCfgNode(){
	return cfgNode;
}

void Node::setRoot(Node* node) {
	root = node;
}

void Node::setExpParent(Node* node) {
	expParent = node;
}

void Node::setParent(Node* node) {
	parent = node;
}

void Node::setLeftChild(Node* node) {
	leftChild = node;
	node->setParent(this);
}

void Node::setRightChild(Node* node) {
	rightChild = node;
	node->setParent(this);
}

void Node::setThirdChild(Node* node) {
	thirdChild = node;
	node->setParent(this);
}

void Node::addStmt(Node* node) {
	indexLst[node] = stmtLst.size() + 1;
	stmtLst[stmtLst.size() + 1] = node; //magic number!!
	node->setParent(this);
}

void Node::setCfgNode(CFGNode* cfg){
	cfgNode = cfg;
}

void Node::debug() {
	std::cout << "Node Type: " << syntType << ", line: " << line << "\n";
}