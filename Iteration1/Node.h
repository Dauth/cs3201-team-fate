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
	Expression* expr;
	
public:
	Node (synt_type, std::string);
	std::string getId();
	Node* getParent();
	Node* getLeftChild();
	Node* getRightChild();
	Node* getstmtLst();
	std::vector<Node*> getStmtLst();
	Variable getVar();
	Expression getExpr();
	
	void setParent(Node*);
	void setLeftChild(Node*);
	void setRightChild(Node*);
	void addStmt(Node*);
	void setVar(Variable*);
	void setExpr(Expression*);
};

#endif