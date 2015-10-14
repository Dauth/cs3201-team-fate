#ifndef CFGNODE_H
#define CFGNODE_H

#include "stdafx.h"
#include "Node.h"
 
class CFGNode{
	Node* statement;
	CFGNode* nextNode;
	std::vector<CFGNode*> prevNode;
	CFGNode* exNextNode;
	CFGNode* endLeft;
	CFGNode* endRight;
	
public:
	CFGNode(Node*);
	Node* getStatement();
	CFGNode* getNextNode();
	CFGNode* getExNextNode();
	CFGNode* getEndLeft();
	CFGNode* getEndRight();
	std::vector<CFGNode*> getPrevNode();

	void setNextNode(CFGNode*);
	void setExNextNode(CFGNode*);
	void setPrevNode(CFGNode*);

	void setEndLeft(CFGNode*);
	void setEndRight(CFGNode*);

};

#endif