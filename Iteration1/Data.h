#ifndef DATA_H
#define DATA_H

#include "stdafx.h"
#include "Node.h"

class Data{

private:
	std::string variable;
	synt_type variableType;
	bool result;
	std::vector<Node*> pkbresult;

public:
	Data(std::string x, synt_type y, bool res);
	//Attributes
	void setVar(std::string x);
	void setVarType(synt_type y);
	void setResult(bool r);
	void setPKBOutput(std::vector<Node*> PKBres);
	std::string getVar();
	synt_type getVarType();
	bool getResult();
	std::vector<Node*> getPKBOutput();

};

#endif