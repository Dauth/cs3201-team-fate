#ifndef PARAMNODE_H
#define PARAMNODE_H

#include "stdafx.h"
#include "AttrType.h"

using namespace std;

class ParamNode {
	SyntType syntType;
	AttrType attrType;
	string param;

public:
	ParamNode(SyntType, AttrType, string);
	SyntType getType();
	AttrType getAttrType();
	string getParam();
};

#endif