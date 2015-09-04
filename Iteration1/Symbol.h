#ifndef SYMBOL_H
#define SYMBOL_H

#include "stdafx.h"
#include "Data.h"

class Symbol{
 private:
      std::multimap<int,Data> myMap;

public:
	Symbol(int i, std::string var, std::string varType);
	~Symbol();
	//attributes
	void setVar(int i, std::string var, std::string varType);
	std::vector<Data> getData(int i);
	int getSize();
};

#endif