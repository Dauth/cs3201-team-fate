#ifndef PARAMNODE_H
#define PARAMNODE_H

#include "stdafx.h"
#include "AttrType.h"

class ParamNode {
	SyntType syntType;
	AttrType attrType;
	std::string param;

public:
	ParamNode(SyntType, AttrType, std::string);
	SyntType getType();
	AttrType getAttrType();
	std::string getParam();
};

#endif