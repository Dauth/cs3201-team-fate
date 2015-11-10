#include "stdafx.h"
#include "CFG.h"

CFG::CFG(){
}
/**The method used to retrieve CFG correspond to a procedure. Return
the first node of the CFG. Return null if not found
Pre: name - Name of the procedure
Post: CFGNode* - the first node of the cfg
*/
CFGNode* CFG::getCFG(std::string name){
	for(unsigned i = 0; i < cfgs.size(); i++){
		if(cfgs.at(i)->getStatement()->getValue() == name){
			return cfgs.at(i);
		}
	}
	return nullptr;
}

/** The method used to build CFG for a procedure. 
Pre: node - the procedure node
*/
void CFG::buildCFG(Node* node){
	CFGNode* startNode = new CFGNode(node);
	CFGNode* currentNode = startNode;
	Node* stmtLst = node->getLeftChild();
	std::map<int,Node*> temp = stmtLst->getStmtLst();
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

/** The method used to create normal CFG node(call or assignment) using
the AST node provided.Return the CFG node after creation
Pre: node - the AST node
Post: newNode - the CFGNode* created
*/
CFGNode* CFG::createNorm(Node* node){
	CFGNode* newNode = new CFGNode(node);
	return newNode;
}

/** The method used to create CFG of a while loop using
the AST node of the while statement provided.Return the 
while CFG node after creation
Pre: node - the AST node of while statement
Post: whileNode - the while CFG node of the loop
*/
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
	linkNode(currentNode,whileNode);
	return whileNode;
}

/** The method used to create the left branch of an if statement. Return the last
CFG node of the branch
Pre: node - the AST node of if statement
Post: currentNode - the last node of the branch
*/
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

/** The method used to create the right branch of an if statement. Return the last
CFG node of the branch
Pre: node - the AST node of if statement
Post: currentNode - the last node of the branch
*/
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

/** The method is the centralized method to create a new CFG node given an AST node.
Depends on the type of the AST node, an appropriate CFGNode is created. Return the CFGNode*
created
Pre: node - the AST node provided
Post: nextNode - the CFG node created
*/
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

/** The method used to link 2 CFG node together. The one before will set its next node 
to be the after node. The node after will set previous node to be the before node.
This is to complete the CFG graph as well as aid with traverse later on.
Pre: currentNode - the node before
	 nextNode - the node after
*/
void CFG::linkNode(CFGNode* currentNode,CFGNode* nextNode){
	if(currentNode->getStatement()->getType() == ifelse){ //if the currentNode is ifelse, we set next to the left and right end instead
		linkNode(currentNode->getEndLeft(),nextNode);
		linkNode(currentNode->getEndRight(),nextNode);
	}
	else{
		currentNode->setNextNode(nextNode);
		nextNode->setPrevNode(currentNode);
		createAndAddPair(currentNode,nextNode);
	}
}

/** The method used to create a pair of two CFG nodes that are
neighbor of each other. This pair will be added to vector of all pairs
This is to aid with the retrieval of all next pair
Pre: currentNode - the node before
	 nextNode - the node after
*/
void CFG::createAndAddPair(CFGNode* currentNode,CFGNode* nextNode){
	Node* current = currentNode->getStatement();
	Node* next = nextNode->getStatement();
	pair<string, string> p (current->getLine(),next->getLine());
	pairs.push_back(p);
}

/** The method used to get the vector contains all pairs in all CFG graphs
Post: pair - the vectore contains all graph
*/
vector<pair<string, string>> CFG::getAllPairs(){
	return pairs;
}