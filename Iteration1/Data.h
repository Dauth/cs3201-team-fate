#ifndef DATA_H
#define DATA_H
#include<iostream>
#include<string>


using namespace std;
class Data{

private:
	std::string variable;
	std::string variableType;
	bool        result;
	//PKB         output;    // I suppose PKB is an already defined type

public:
	~Data();
	Data(string x,string y,bool res);
	//Attributes
	void setVar(string x);
	void setVarType(string y);
	void setResult(bool r);
	string getVar();
	string getVarType();
	bool getResult();

};

#endif