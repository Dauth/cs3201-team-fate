#ifndef NODE_H
#define NODE_H

#include "stdafx.h"

class CFGNode;

class Node {
	SyntType syntType; // type of node
	std::string line; // prog/statement line
	std::string value; // constant value, procedure name, call target, expr
	Node* root; // pointer to procedure node
	Node* expParent;//pointer to parent of exp node
	Node* parent; // parent as in tree parent, not parent(s1,s2) function
	Node* leftChild; 
	Node* rightChild;
	Node* thirdChild;
	std::map<int, Node*> stmtLst;
	std::map<Node*, int> indexLst;
	CFGNode* cfgNode;

	
public:
	Node (SyntType, std::string, std::string);
	Node (SyntType, std::string);
	SyntType getType();
	std::string getLine();
	bool isStatement();
	bool isContainer();
	std::string getValue();
	Node* getRoot();
	Node* getExpParent();
	Node* getParent();
	Node* getLeftChild();
	Node* getRightChild();
	Node* getThirdChild();
	CFGNode* getCfgNode();
	std::map<int, Node*> getStmtLst();
	std::map<Node*, int> getIndexLst();
	
	void setRoot(Node*);
	void setExpParent(Node*);
	void setParent(Node*);
	void setLeftChild(Node*);
	void setRightChild(Node*);
	void setThirdChild(Node*);
	void addStmt(Node*);
	void setCfgNode(CFGNode*);

	void debug();
};

#endif