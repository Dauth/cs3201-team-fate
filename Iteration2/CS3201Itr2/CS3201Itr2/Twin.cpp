#include "stdafx.h"
#include "Node.h"
#include "Twin.h"

Twin::Twin (Node* stm, Node* stmLst) {
	stmNode = stm;
	stmListNode = stmLst;
}

Node* Twin::getStmNode() {
	return stmNode;
}

Node* Twin::getStmListNode() {
	return stmListNode;
}