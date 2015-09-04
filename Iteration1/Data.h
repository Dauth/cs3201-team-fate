#ifndef DATA_H
#define DATA_H

#include "stdafx.h"
#include "Node.h"

class Data{

private:
	std::string variable;
	std::string variableType;
	bool result;
	std::vector<Node*> pkbresult;

public:
	Data(std::string x, std::string y, bool res);
	~Data();
	//Attributes
	void setVar(std::string x);
	void setVarType(std::string y);
	void setResult(bool r);
	std::string getVar();
	std::string getVarType();
	bool getResult();

};

#endif