#include "stdafx.h"
#include "QueryNode.h"

QueryNode::QueryNode (query_type type, ParamNode* left, ParamNode* right) {
	QUERY_TYPE = type;
	leftParam = left;
	rightParam = right;
	nextQuery = NULL;
}

query_type QueryNode::getType() {
	return QUERY_TYPE;
}

QueryNode* QueryNode::getNext() {
	return nextQuery;
}

ParamNode* QueryNode::getLeftParam() {
	return leftParam;
}

ParamNode* QueryNode::getRightParam() {
	return rightParam;
}

void QueryNode::setNext(QueryNode* query) {
	nextQuery = query;
}