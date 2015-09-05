#include "stdafx.h"
#include "Data.h"
#include"Node.h"

//TODO add the PKB object
//leave this empty
Data::Data(std::string var, synt_type varType, bool res=false) //, PKB out = nullptr
{
	variable=var;
	variableType=varType;
	result=res;
}


//SETTERS
void Data::setVar(std::string var1)
{
	variable=var1;
}
void Data::setVarType(synt_type varType)
{
	variableType=varType;
}
void Data::setResult(bool res1)
{
	result=res1;
}
void Data::setPKBOutput(std::vector<Node*> PKBres)
{
	pkbresult=PKBres;
}
//GETTERS
std::string Data::getVar()
{
	return variable;
}
synt_type Data::getVarType()
{
	return variableType;
}
bool Data::getResult()
{
	return result;
}
std::vector<Node*> Data::getPKBOutput()
{
	return pkbresult;
}
