#include "stdafx.h"
#include "Symbol.h"
#include<string>
#include "Data.h"

Symbol::Symbol()
{
	std::vector<Data*> symbolTable;
}
Symbol::Symbol(std::string var, synt_type varType )
{
	symbolTable.push_back(new Data(var,varType,false));
}
void Symbol::setVar(std::string var, synt_type varType)
{
	symbolTable.push_back(new Data(var,varType,false));
}
void Symbol::setResult(int index)
{
	symbolTable.at(index)->setResult(true);
}
std::vector<Data*> Symbol::getQuery()
{	
	return symbolTable;
}
int Symbol::getSize()
{
	return symbolTable.max_size();
}
int Symbol::getIndex(std::string var)
{
	for(std::vector<Data*>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
	{
		if((**i).getVar()==var)
			return std::distance(symbolTable.begin(), i);
	}
}
bool Symbol::exists(std::string var)
{
	bool found = false;
	for(std::vector<Data*>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
	{
		if((**i).getVar()==var)
		{
			found = true;
		}
			
	}
	return found;
}


