#include "stdafx.h"
#include "ParamNode.h"

ParamNode::ParamNode(SyntType type, AttrType aType, string p) {
	syntType = type;
	attrType = aType;
	param = p;
}

SyntType ParamNode::getType() {
	return syntType;
}

AttrType ParamNode::getAttrType() {
	return attrType;
}

string ParamNode::getParam() {
	return param;
}