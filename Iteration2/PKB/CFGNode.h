#ifndef CFGNODE_H
#define CFGNODE_H

#include "stdafx.h"
#include "Node.h"
 
class CFGNode{
	Node* statement;
	CFGNode* nextNode;
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

	void setNextNode(CFGNode*);
	void setExNextNode(CFGNode*);

	void setEndLeft(CFGNode*);
	void setEndRight(CFGNode*);

};

#endif