#ifndef PARAMNODE_H
#define PARAMNODE_H

#include "stdafx.h"
#include "synt_type.h"

class ParamNode {

public:
	ParamNode (synt_type, std::string);
	synt_type getType();
	std::string getParam();
};

#endif