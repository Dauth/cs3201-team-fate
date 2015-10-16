#ifndef SIMPLEPARSER_H
#define SIMPLEPARSER_H

#include "stdafx.h"
#include "AST.h"
#include <string>
#include <vector>


class SimpleParser {
	AST* ast;

private:

public:
	SimpleParser(AST*);
	std::vector<std::string>SimpleParser::readSourceFile(std::string);
	std::vector<Node*>SimpleParser::execute(std::vector<std::string>);

};

#endif