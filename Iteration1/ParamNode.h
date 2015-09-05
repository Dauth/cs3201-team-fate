#ifndef PARAMNODE_H
#define PARAMNODE_H

#include "stdafx.h"

class ParamNode {
	synt_type SYNT_TYPE;
	std::string param;

public:
	ParamNode (synt_type, std::string);
	synt_type getType();
	std::string getParam();
};

#endif