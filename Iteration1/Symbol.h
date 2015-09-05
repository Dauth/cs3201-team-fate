#ifndef SYMBOL_H
#define SYMBOL_H

#include "stdafx.h"
#include "Data.h"

class Symbol{
private:
	std::vector<Data*> symbolTable;

public:
	Symbol();
	Symbol(std::string var, synt_type varType);
	//attributes
	void setVar(std::string var, synt_type varType);
	void setResult(int index);
	std::vector<Data*>& getQuery();
	int getSize();
	int getIndex(std::string var);
	bool exists(std::string var);
};

#endif