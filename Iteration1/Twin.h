#ifndef TWIN_H
#define TWIN_H

#include "stdafx.h"

class Twin {
	Node* stmNode;
	Node* stmListNode;

	
public:
	Twin (Node*, Node*);
	Node* getStmNode();
	Node* getStmListNode();
};

#endif