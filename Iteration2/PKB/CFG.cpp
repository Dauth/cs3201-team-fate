#include "stdafx.h"
#include "CFG.h"
#include "PKB.h"

CFG::CFG(){
}

CFGNode* CFG::getCFG(std::string name){
	for(unsigned i = 0; i < cfgs.size(); i++){
		if(cfgs.at(i)->getStatement()->getValue() == name){
			return cfgs.at(i);
		}
	}
	return ;
}
void CFG::buildCFG(Node* node){
	CFGNode* startNode = new CFGNode(node);
	CFGNode* currentNode = startNode;
	std::map<int,Node*> temp = node->getStmtLst();
	for(unsigned i = 1; i <= temp.size();i++){
		CFGNode* nextNode = createNext(temp.at(i));

		if(currentNode->getStatement()->getType() == ifelse){ //if the currentNode is ifelse, we set next to the left and right end instead
				currentNode->getEndLeft()->setNextNode(nextNode);
				currentNode->getEndRight()->setNextNode(nextNode);
		}
		else{
			currentNode->setNextNode(nextNode);
		}
		
		currentNode = nextNode;
	}
	CFGNode* endNode;
	currentNode->setNextNode(endNode);
	cfgs.push_back(startNode);
}

CFGNode* CFG::createNorm(Node* node){
	CFGNode* newNode = new CFGNode(node);
	return newNode;
}

CFGNode* CFG::createWhile(Node* node){
	CFGNode* whileNode = new CFGNode(node);
	std::map<int,Node*> temp = node->getRightChild()->getStmtLst();
	CFGNode* currentNode = whileNode;
	for(unsigned i = 1; i <= temp.size(); i++){
		CFGNode* nextNode = createNext(temp.at(i));

		//set next Node. If it is the first iteration,setExNext since it is the node after while Node
		if(i == 1){
			currentNode->setExNextNode(nextNode);
		}
		else{ 
			if(currentNode->getStatement()->getType() == ifelse){ 
				currentNode->getEndLeft()->setNextNode(nextNode);
				currentNode->getEndRight()->setNextNode(nextNode);
			}
			else{
				currentNode->setNextNode(nextNode);
			}
		}

		currentNode = nextNode;
	}
	currentNode->setNextNode(whileNode);
	return whileNode;
}

CFGNode* CFG::createIfLeft(CFGNode* node){
	std::map<int,Node*> temp = node->getStatement()->getRightChild()->getStmtLst();
	CFGNode* currentNode = node;
	for(unsigned i = 1; i <= temp.size(); i++){
		CFGNode* nextNode = createNext(temp.at(i));
		
		//set next Node. If it is the first iteration,setNext since it is the node after if Node
		if(i == 1){
			currentNode->setNextNode(nextNode);
		}
		else{
			if(currentNode->getStatement()->getType() == ifelse){
				currentNode->getEndLeft()->setNextNode(nextNode);
				currentNode->getEndRight()->setNextNode(nextNode);
			}
			else{
				currentNode->setNextNode(nextNode);
			}
		}
		
		currentNode = nextNode;
	}
	return currentNode;
}

CFGNode* CFG::createIfRight(CFGNode* node){
	std::map<int,Node*> temp = node->getStatement()->getThirdChild()->getStmtLst();
	CFGNode* currentNode = node;
	for(unsigned i = 1; i <= temp.size(); i++){
		CFGNode* nextNode = createNext(temp.at(i));

		//set next Node. If it is the first iteration,setExNext since it is the node after else Node
		if(i == 1){
			currentNode->setExNextNode(nextNode);
		}
		else{
			if(currentNode->getStatement()->getType() == ifelse){
				currentNode->getEndLeft()->setNextNode(nextNode);
				currentNode->getEndRight()->setNextNode(nextNode);
			}
			else{
				currentNode->setNextNode(nextNode);
			}
		}
		currentNode = nextNode;
	}
	return currentNode;
}

CFGNode* CFG::createNext(Node* node){
	CFGNode* nextNode;
	if(node->getType() == whileLoop){
		nextNode = createWhile(node);
	}
	else if(node->getType() == ifelse){
		nextNode = new CFGNode(node);
		nextNode->setEndLeft(createIfLeft(nextNode));
		nextNode->setEndRight(createIfRight(nextNode));
	}
	else{
		nextNode = createNorm(node);
	}
	
	return nextNode;
}