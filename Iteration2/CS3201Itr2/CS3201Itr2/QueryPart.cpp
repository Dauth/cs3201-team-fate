#include "stdafx.h"
#include "QueryPart.h"

QueryPart::QueryPart(QueryType type, ParamNode* left, ParamNode* right, ParamNode* last) {
	queryType = type;
	leftParam = left;
	rightParam = right;
	lastParam = last;
}

QueryType QueryPart::getType() {
	return queryType;
}

ParamNode* QueryPart::getLeftParam() {
	return leftParam;
}

ParamNode* QueryPart::getRightParam() {
	return rightParam;
}

ParamNode* QueryPart::getLastParam() {
	return lastParam;
}