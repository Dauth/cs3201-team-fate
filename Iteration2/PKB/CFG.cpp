#include "stdafx.h"
#include "CFG.h"
#include "PKB.h"

using namespace std;

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

		if(i == 1){
			currentNode->setNextNode(nextNode);
		}
		else{
			linkNode(currentNode,nextNode);
		}
		
		currentNode = nextNode;
	}
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
			nextNode->setPrevNode(currentNode);
			createAndAddPair(currentNode,nextNode);
		}
		else{ 
			linkNode(currentNode,nextNode);
		}

		currentNode = nextNode;
	}
	currentNode->setNextNode(whileNode);
	whileNode->setPrevNode(currentNode);
	createAndAddPair(currentNode,whileNode);
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
			nextNode->setPrevNode(currentNode);
			createAndAddPair(currentNode,nextNode);
		}
		else{
			linkNode(currentNode,nextNode);
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
			nextNode->setPrevNode(currentNode);
			createAndAddPair(currentNode,nextNode);
		}
		else{
			linkNode(currentNode,nextNode);
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
	node->setCfgNode(nextNode);
	return nextNode;
}

void CFG::linkNode(CFGNode* currentNode,CFGNode* nextNode){
	if(currentNode->getStatement()->getType() == ifelse){ //if the currentNode is ifelse, we set next to the left and right end instead
		currentNode->getEndLeft()->setNextNode(nextNode);
		currentNode->getEndRight()->setNextNode(nextNode);
		nextNode->setPrevNode(currentNode->getEndLeft());
		nextNode->setPrevNode(currentNode->getEndRight());
		createAndAddPair(currentNode->getEndLeft(),nextNode);
		createAndAddPair(currentNode->getEndRight(),nextNode);
	}
	else{
		currentNode->setNextNode(nextNode);
		nextNode->setPrevNode(currentNode);
		createAndAddPair(currentNode,nextNode);
	}
}

void CFG::createAndAddPair(CFGNode* currentNode,CFGNode* nextNode){
	Node* current = currentNode->getStatement();
	Node* next = nextNode->getStatement();
	pair<string, string> p (current->getLine(),next->getLine());
	pairs.push_back(p);
}

vector<pair<string, string>> CFG::getAllPairs(){
	return pairs;
}