#include "stdafx.h"
#include "ParamNode.h"

ParamNode::ParamNode (synt_type type, std::string p) {
	SYNT_TYPE = type;
	param = p;
}

synt_type ParamNode::getType() {
	return SYNT_TYPE;
}

std::string ParamNode::getParam() {
	return param;
}