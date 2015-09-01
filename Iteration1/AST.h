#ifndef AST_H
#define AST_H

#include "stdafx.h"
#include "Node.h"

class AST {

	Node root;
	int size;
	int height;

public:
	AST(Node);
	Node* getRoot();
	int getSize();
	int getHeight();

};

#endif