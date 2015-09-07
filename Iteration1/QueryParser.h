#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include "stdafx.h"
#include <string>
#include <vector>
#include <fstream>
#include "QueryEvaluator.h"

class QueryParser {
	QueryEvaluator* qe;

private:

public:
	QueryParser(QueryEvaluator qe);//replaced : QueryParser(qe*)
	std::vector<std::string>QueryParser::readSourceFile(std::string);
	bool verifyCorrectParameters(synt_type currentSyn, std::string firstParam, std::string secondParam, std::string thirdParam);
	bool verifyCorrectParameters(query_type queryClause,std::string firstParam, std::string secondParam);
	synt_type getSynType (std::string synType);
	query_type getType (char* queryType);
	void removeCharsFromString( std::string &str, char* charsToRemove );
	std::vector<std::string> split(const char *str, char c = ' ');
	void ProcessEachToken(char *currentToken);
	int main();
	void Match (char *c);
	void print(std::vector <std::string> & v );


};
#endif