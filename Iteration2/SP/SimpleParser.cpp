#include "stdafx.h"
#include <string>
#include <vector>
#include <fstream> 
#include "AST.h"
#include "SimpleParser.h"


using namespace std;

SimpleParser::SimpleParser(AST* a){
	ast = a;
}

std::vector<std::string> SimpleParser::readSourceFile(std::string sourceFile){
	std::vector<string> sourceVector;
	std::ifstream infile(sourceFile);
	std::string line;
	
	while(std::getline(infile, line)){
		if(!line.empty()){
			sourceVector.push_back(line);
		}
	}
	return sourceVector;
}

std::vector<Node*> SimpleParser::execute(std::vector<string> sourceVector){
	return ast->buildAST(sourceVector);
}