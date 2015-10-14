#include "stdafx.h"
#include "CFGNode.h"
#include "Node.h"

CFGNode::CFGNode (Node* node){
	statement = node;
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

void CFGNode::setExNextNode(CFGNode* exNext){
	exNextNode = exNext;
}

void CFGNode::setNextNode(CFGNode* next){
	nextNode = next;
}

void CFGNode::setEndLeft(CFGNode* end){
	endLeft = end;
}

void CFGNode::setEndRight(CFGNode* end){
	endRight = end;
}