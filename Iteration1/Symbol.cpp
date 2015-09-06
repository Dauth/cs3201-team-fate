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
std::vector<Data*>& Symbol::getQuery()
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
	return -1;
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
synt_type Symbol::getSyntType(std::string var)
{
	synt_type toReturn = synt_type::errorr;

	for(std::vector<Data*>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
	{
		if((**i).getVar()==var)
		{
			Data *newData;
			newData = *i;
			toReturn = newData->getVarType();
		}
			
	}
	return toReturn;


}


