#include "QueryNode.h"

query_type QUERY_TYPE;
ParamNode* leftParam;
ParamNode* rightParam;
QueryNode* nextQuery;

QueryNode::QueryNode (query_type queryType, ParamNode* leftParam, ParamNode* rightParam) {
	QUERY_TYPE = queryType;
	leftParam = leftParam;
	rightParam = rightParam;
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

void QueryNode::setNext(QueryNode* queryNode) {
	nextQuery = queryNode;
}