#include<string>
#include<vector>
#include <fstream> 



using namespace std;

SimpleParser::SimpleParser(){}
std::vector<Node*> tree;


std::vector<string> readSourceFile(std:;string sourceFile){
	std::vector<string> sourceVector;
	std::string line;

	while(getLine(sourceFile, line){
		if(!line.empty()){
			sourceVector.push_back(line);
		}
	}
	return sourceVector;
}

void execute(std::vector<string> sourceVector){
	tree = buildAST(sourceVector);
}