#include <string>
#include<vector>
#include "SymbolTable.h"

using namespace std;

	//SETTERS
	/*for declaration*/
	void SymbolTable::addVar(string var_value,string var_type,int QueryID)
	{
		QID.push_back(QueryID);
		var.push_back(var_value);
		assign.push_back(var_type);
		result.push_back(false);
		//PKBoutput.push_back(null);
	
	}
	/*If var_value exist, set result=true*/
	vector<int> SymbolTable::retrieveIndex(int QueryID)
	{
	 //TODO iterate through
	}
	void SymbolTable::addResult (string var_value)
	{
		//vector<string>::iterator it;
		for(int n=0; n< var.size(); n++)
		{
			if(var[n]==var_value){
				result[n]=true;
			}
		}
	}
	/**/
	//TODO
	/*void SymbolTable::addPKBoutput(PK output )
	{
		//intersect the object
	}*/
	
	//GETTERS
	vector<string> SymbolTable::getVariable()
	{
		return var;
	}
	string SymbolTable::getVariable(int index)
	{
		return var[index];
	}
	vector <string> SymbolTable:: getAssignment()
	{
		return assign;
	}
	string SymbolTable:: getAssignment(int index )
	{
		return assign[index];
	}
	vector<bool> SymbolTable:: getResult()
	{
		return result;
	}
	bool SymbolTable:: getResult(int index)
	{
		return result[index];
	}
	/*vector <PK> SymbolTable::getPKBoutput 
	{
	return PKBoutput;

	it will only be one object.
	}	
	*/