#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include "stdafx.h"
#include "QueryObject.h"

class QueryParser {

public:
	QueryParser ();
	QueryObject QueryParser::getQueryObject(std::string);
};

#endif