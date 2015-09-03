#pragma once // for multiple calls of header

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <iostream>
#include <string>
#include<vector>

/* iNSTRUCTIONS TO USE
	-just write header file into cppfile
	-create object by writing "SymbolTable T" where T can be any letter
*/
using namespace std;

class SymbolTable
{
	private:
		/*Constructor creates 5 vectors of type*/
		/**QueryID,variable,type of variable,result, PKBoutput**/
		/*Link them using index*/
		std :: vector<int> QID;
		std :: vector<string> var;
		std :: vector<string> assign;
		std :: vector<bool> result;
		//std :: vector<PK> PKBoutput;

	public:
		void addVar(string var_value,string var_type, int QueryID);
		vector<int>retrieveIndex(int QueryID);
		void addResult (string var_value);
		vector<string> getVariable();
		vector <string> getAssignment();
		vector<bool> getResult();
		string getVariable(int index);
		string getAssignment(int index );
		bool getResult(int index);
		//TODO addPKBOutput() method;
		//TODO getPKBOutput();

};

#endif