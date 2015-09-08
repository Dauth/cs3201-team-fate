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
	QueryParser(qe*);
	std::list<std::string>QueryParser::readSourceFile(std::string);

#endif