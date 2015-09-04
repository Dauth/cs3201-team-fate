#include "stdafx.h"
#include "Symbol.h"
#include<string>

Symbol::Symbol(std::string var, std::string varType )
{
	SymbolTable.push_back(Data(var,varType,false));
}
void Symbol:: setVar(std::string var, std::string varType)
{
	SymbolTable.push_back(Data(var,varType,false));
}
void Symbol:: setResult(int index)
{
	SymbolTable[index].setResult(true);
}
std::vector<Data> Symbol::getQuery()
{	
	return SymbolTable;
}
int Symbol::getSize()
{
	return SymbolTable.max_size();
}
int Symbol::getIndex(std::string var)
{
	for(int i=0; i<SymbolTable.max_size();i++)
	{
		if(SymbolTable[i].getVar()==var)
			return i;
	}
}


