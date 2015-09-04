#ifndef DATA_H
#define DATA_H

#include "stdafx.h"

class Data{

private:
	std::string variable;
	std::string variableType;
	bool result;
	std::vector<Node*> pkbresult;

public:
	Data(string x,string y,bool res);
	~Data();
	//Attributes
	void setVar(string x);
	void setVarType(string y);
	void setResult(bool r);
	std::string getVar();
	std::string getVarType();
	bool getResult();

};

#endif