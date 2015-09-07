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
	QueryParser(qe*);
	std::vector<std::string>QueryParser::readSourceFile(std::string);

};
#endif