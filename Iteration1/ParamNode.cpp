#include "ParamNode.h"

synt_type SYNT_TYPE;
std::string param;

ParamNode::ParamNode (synt_type type, std::string param) {
	SYNT_TYPE = type;
	param = param;
}

synt_type ParamNode::getType() {
	return SYNT_TYPE;
}

std::string ParamNode::getParam() {
	return param;
}