#include "Data.h"
#include "Symbol.h"
#include <iostream>
#include<vector>

using namespace std;

Symbol::~Symbol()
{
}
Symbol::Symbol(int i, string var, string varType )
{
	myMap.insert(std::make_pair(i, Data(var,varType,false)));
}
void Symbol:: setVar(int i,string var, string varType)
{
	myMap.insert(std::make_pair(i, Data(var,varType,false)));
}
vector<Data> Symbol:: getData(int i)
{
	vector<Data> vec; 
	//multimap<int, Data>::iterator iter=myMap.find(i);
	//give by batch
	multimap<int, Data>::const_iterator it =myMap.lower_bound(i);
	multimap<int, Data>::const_iterator it2 =myMap.upper_bound(i);
	while(it!=it2)
	{
		vec.push_back(it->second);
		++it;
	}
	return vec;
}
int Symbol:: getSize()
{
	return myMap.max_size();
}
