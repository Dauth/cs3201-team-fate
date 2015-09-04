#include "stdafx.h"
#include "Data.h"
#include"Node.h"

//TODO add the PKB object
//leave this empty
Data::Data(std::string var, std::string varType, bool res=false) //, PKB out = nullptr
{
	variable=var;
	variableType=varType;
	result=res;
	//, output(out)
}


//SETTERS
void Data::setVar(std::string var1)
{
	variable=var1;
}
void Data::setVarType(std::string varType)
{
	variableType=varType;
}
void Data::setResult(bool res1)
{
	result=res1;
}
void Data::setPKBoutput(std::vector<Node*> PKBres)
{
	pkbresult=PKBres;
}
//GETTERS
std::string Data::getVar()
{
	return variable;
}
std::string Data::getVarType()
{
	return variableType;
}
bool Data::getResult()
{
	return result;
}
std::vector<Node*> Data::getPKBoutput()
{
	return pkbresult;
}
