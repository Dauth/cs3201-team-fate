#include "stdafx.h"
#include "CFGNode.h"
#include "Node.h"

CFGNode::CFGNode (Node* node){
	statement = node;
	nextNode = NULL;
	exNextNode = NULL;
}

Node* CFGNode::getStatement(){
	return statement;
}

CFGNode* CFGNode::getExNextNode(){
	return exNextNode;
}

CFGNode* CFGNode::getNextNode(){
	return nextNode;
}

CFGNode* CFGNode::getEndLeft(){
	return endLeft;
}

CFGNode* CFGNode::getEndRight(){
	return endRight;
}

std::vector<CFGNode*> CFGNode::getPrevNode(){
	return prevNode;
}

void CFGNode::setExNextNode(CFGNode* exNext){
	exNextNode = exNext;
}

void CFGNode::setNextNode(CFGNode* next){
	nextNode = next;
}

void CFGNode::setPrevNode(CFGNode* prev){
	prevNode.push_back(prev);
}

void CFGNode::setEndLeft(CFGNode* end){
	endLeft = end;
}

void CFGNode::setEndRight(CFGNode* end){
	endRight = end;
}