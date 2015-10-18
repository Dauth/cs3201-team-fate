#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include "stdafx.h"
#include "Query_Object.h"

class QueryParser {

public:
	QueryParser ();
	Query_Object QueryParser::getQueryObject(std::string);
};

#endif