#include "stdafx.h"
#include "PatternNode.h"

PatternNode::PatternNode (ParamNode* left, ParamNode* middle, ParamNode* right) {
	leftParam = left;
	middleParam = middle;
	rightParam = right;
	nextPattern = NULL;
}

PatternNode* PatternNode::getNext() {
	return nextPattern;
}

ParamNode* PatternNode::getLeftParam() {
	return leftParam;
}

ParamNode* PatternNode::getMiddleParam() {
	return middleParam;
}

ParamNode* PatternNode::getRightParam() {
	return rightParam;
}

void PatternNode::setNext(PatternNode* pattern) {
	nextPattern = pattern;
}