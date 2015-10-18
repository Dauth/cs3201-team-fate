#include "stdafx.h"
#include "Symbol.h"
#include<string>

Symbol::Symbol()
{
	std::vector<std::pair<std::string,SyntType>> symbolTable;
}
Symbol::Symbol(std::string var, SyntType varType )
{
	std::pair<std::string,SyntType> newPair;
	symbolTable.push_back(std::make_pair(var,varType));
}
void Symbol::setVar(std::string var, SyntType varType)
{
	symbolTable.push_back(std::make_pair(var,varType));
}
std::vector<std::pair<std::string,SyntType>> Symbol::getQuery()
{	
	return symbolTable;
}
int Symbol::getSize()
{
	return symbolTable.max_size();
}
int Symbol::getIndex(std::string var)
{
	for(std::vector<std::pair<std::string,SyntType>>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
	{
		if((*i).first==var)
			return std::distance(symbolTable.begin(), i);
	}
	return -1;
}
bool Symbol::exists(std::string var)
{
	bool found = false;
	for(std::vector<std::pair<std::string,SyntType>>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
	{
		if((*i).first==var)
		{
			found = true;
		}
			
	}
	return found;
}
SyntType Symbol::getSyntType(std::string var)
{
	SyntType toReturn = synError;

	for(std::vector<std::pair<std::string,SyntType>>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
	{
		if((*i).first==var)
		{
			toReturn = (*i).second;
		}
			
	}
	return toReturn;


}


