#include "stdafx.h"
#include "Symbol.h"

Symbol::~Symbol()
{
}
Symbol::Symbol(int i, std::string var, std::string varType )
{
	myMap.insert(std::make_pair(i, Data(var,varType,false)));
}
void Symbol:: setVar(int i,std::string var, std::string varType)
{
	myMap.insert(std::make_pair(i, Data(var,varType,false)));
}
std::vector<Data> Symbol::getData(int i)
{
	std::vector<Data> vec; 
	//multimap<int, Data>::iterator iter=myMap.find(i);
	//give by batch
	std::multimap<int, Data>::const_iterator it =myMap.lower_bound(i);
	std::multimap<int, Data>::const_iterator it2 =myMap.upper_bound(i);
	while(it!=it2)
	{
		vec.push_back(it->second);
		++it;
	}
	return vec;
}
int Symbol::getSize()
{
	return myMap.max_size();
}
