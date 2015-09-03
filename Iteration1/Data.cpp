#include<iostream>
#include <map>
#include <string>
#include "Data.h"

using namespace std;

//TODO add the PKB object
//leave this empty
Data::~Data()
{
}
Data::Data(string var, string varType, bool res=false) //, PKB out = nullptr
{
	variable=var;
	variableType=varType;
	result=res;
	//, output(out)
}


//SETTERS
void Data::setVar(string var1)
{
	variable=var1;
}
void Data::setVarType(string varType)
{
	variableType=varType;
}
void Data::setResult(bool res1)
{
	result=res1;
}
//GETTERS
string Data::getVar()
{
	return variable;
}
string Data::getVarType()
{
	return variableType;
}
bool Data::getResult()
{
	return result;
}
