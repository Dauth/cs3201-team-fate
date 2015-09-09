#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include "stdafx.h"
#include <string>
#include <vector>
#include "QueryEvaluator.h"

class QueryParser {
	QueryEvaluator* qe;

private:

public:
	QueryParser(QueryEvaluator*);
	// expects input to be a filename.
	void QueryParser::readSourceFile(std::string);

	// expects one query concat together into one line.
	void QueryParser::readQuery(std::string);

};
#endif