#ifndef SYMBOL_H
#define SYMBOL_H

#include "stdafx.h"
#include "Data.h"

using namespace std;
class Symbol{
 private:
      std :: multimap <int,Data> myMap;

public:
	Symbol(int i, string var, string varType);
	~Symbol();
	//attributes
	void setVar(int i, string var, string varType);
	vector<Data> getData(int i);
	int getSize();
};

#endif