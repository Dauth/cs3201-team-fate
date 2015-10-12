#include "stdafx.h"
#include "ParamNode.h"

ParamNode::ParamNode(SyntType type, AttrType aType, std::string p) {
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

std::string ParamNode::getParam() {
	return param;
}