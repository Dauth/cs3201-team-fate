#ifndef NODE_H
#define NODE_H

#include "stdafx.h"
#include "Variable.h"

class Node {
	synt_type SYNT_TYPE;
	std::string identifier;
	Node* root;
	Node* parent;
	Node* leftChild;
	Node* rightChild;
	std::vector<Node*> stmtLst;
	Variable* var;
	
public:
	Node (synt_type, std::string);
	std::string getId();
	Node* getParent();
	Node* getLeftChild();
	Node* getRightChild();
	Node* getstmtLst();
	std::vector<Node*> getStmtLst();
	Variable* getVar();
	
	void setParent(Node*);
	void setLeftChild(Node*);
	void setRightChild(Node*);
	void addStmt(Node*);
	void setVar(Variable*);
};

#endif