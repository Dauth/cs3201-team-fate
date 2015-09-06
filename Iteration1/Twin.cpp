#include "stdafx.h"
#include "Node.h"


Twin::Twin (Node* stm, Node* stmLst) {
	stmNode = stm;
	stmListNode = stmLst
}

Node* Twin::getStmNode() {
	return stmNode;
}

Node* Twin::getStmListNode() {
	return stmListNode;
}