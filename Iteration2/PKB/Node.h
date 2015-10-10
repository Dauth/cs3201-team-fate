#ifndef NODE_H
#define NODE_H

#include "stdafx.h"

class Variable;

class Node {
	synt_type SYNT_TYPE; // type of node
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
	Variable* var;
	
public:
	Node (synt_type, std::string, std::string);
	Node (synt_type, std::string);
	synt_type getType();
	std::string getLine();
	bool isStatement();
	std::string getValue();
	Node* getRoot();
	Node* getExpParent();
	Node* getParent();
	Node* getLeftChild();
	Node* getRightChild();
	Node* getThirdChild();
	std::map<int, Node*> getStmtLst();
	std::map<Node*, int> getIndexLst();
	Variable* getVariable();
	
	void setRoot(Node*);
	void setExpParent(Node*);
	void setParent(Node*);
	void setLeftChild(Node*);
	void setRightChild(Node*);
	void setThirdChild(Node*);
	void addStmt(Node*);
	void setVar(Variable*);

	void debug();
};

#endif