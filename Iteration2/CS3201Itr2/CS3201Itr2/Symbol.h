#ifndef SYMBOL_H
#define SYMBOL_H

#include "stdafx.h"

class Symbol{
private:
	std::vector<std::pair<std::string,SyntType>> symbolTable;

public:
	Symbol();
	Symbol(std::string var, SyntType varType);
	//attributes
	void setVar(std::string var, SyntType varType);
	std::vector<std::pair<std::string,SyntType>> getQuery();
	int getSize();
	int getIndex(std::string var);
	bool exists(std::string var);
	SyntType getSyntType(std::string var);
};

#endif