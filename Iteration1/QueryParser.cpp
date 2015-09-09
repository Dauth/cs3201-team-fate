#include <iostream>
#include <string>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <regex>
#include "QueryTree.h"
#include "QueryNode.h"
#include "Symbol.h"
#include "ParamNode.h"
#include <fstream>
//#include "QueryEvaluator.h"
// LINE 656 is where it is supposed to call evaluate from QueryEvaluator

using namespace std;

namespace TOKEN{
	enum Value { SELECT, RESULT_CL, WITH_CL, SUCHTHAT_CL, PATTERN_CL };
}

TOKEN::Value currToken = TOKEN::SELECT;

bool expectingThat;
bool resultBool;
bool tupleError;
bool nonExistantSyn;
bool suchThatQueryExist;
bool patternExist;
bool suchThatQueryPass;
bool patternPass;

QueryTree* rootTree;
Symbol newSymbol;


string concatStmt = "";
string errorMsg = "ERROR(s) : \n";

//regex stmtRef_old("(Parent|Parent\\*|Affects|Affects\\*|Follows|Follows\\*)\\((([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+),(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)\\)");

// INDENT | "_" | INTEGER
regex stmtRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)");
regex lineRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)");

// INDENT | "_"| "INDENT" | INTEGER
regex entRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+|\"([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*\")");

// INDENT | "_" | "INDENT"
regex varRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|\"([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*\")");


regex expressionSpec("\"(([a-zA-Z])+(([a-zA-Z])|(\\d)+)*|\\d+)((\\+|\\-|\\*)(([a-zA-Z])+(([a-zA-Z])|(\\d)+)*|\\d+))*\"");
regex underScoresBothSides("_\"(([a-zA-Z])+(([a-zA-Z])|(\\d)+)*|\\d+)((\\+|\\-|\\*)(([a-zA-Z])+(([a-zA-Z])|(\\d)+)*|\\d+))*\"_");



bool verifyCorrectParameters(synt_type currentSyn, string firstParam, string secondParam, string thirdParam)
{
	bool valid = false;
	regex underScore ("_");
	if (currentSyn == synt_type::assignment)
	{
		if (regex_match(firstParam,varRef))
		{
			if (regex_match(secondParam,expressionSpec) || regex_match(secondParam,underScoresBothSides))
			{
				valid = true;
			}
			else
			{
				errorMsg += "- Invalid second parameter of the pattern.";
			}
		}
		else
		{
			errorMsg += "- Invalid first parameter of the pattern.";
		}
	}
	else if (currentSyn == synt_type::whileLoop)
	{
		if (regex_match(firstParam,varRef) && regex_match(secondParam, underScore))
		{
			valid = true;
		}
	}
	else if (currentSyn == synt_type::ifelse)
	{
		if (regex_match(firstParam,varRef) && regex_match(secondParam,underScore) && regex_match(thirdParam, underScore))
		{
			valid = true;
		}
	}
	return valid;
}
bool verifyCorrectParameters(query_type queryClause,string firstParam, string secondParam)
{
	bool valid = false;
	if (queryClause == query_type::modifies)
	{
		if (regex_match(firstParam,entRef) && regex_match(secondParam,entRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::uses)
	{
		if (regex_match(firstParam,entRef) && regex_match(secondParam,varRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::calls)
	{
		if (regex_match(firstParam,entRef) && regex_match(secondParam,entRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::callsStar)
	{
		if (regex_match(firstParam,entRef) && regex_match(secondParam,entRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::parent)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::parentStar)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::follows)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::followsStar)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::next)
	{
		if (regex_match(firstParam,lineRef) && regex_match(secondParam,lineRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::nextStar)
	{
		if (regex_match(firstParam,lineRef) && regex_match(secondParam,lineRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::affects)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	else if (queryClause == query_type::affectsStar)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	return valid;
}

synt_type getSynType (string synType)
{
	regex doubleQuotes ("\"[^\"]+\"");
	regex underScoreBothSides("_\"[^\"]+\"_");
	regex underScoreOnly("_");

	regex integer ("\\d+");
	synt_type toReturn = synt_type::synError;

	if (regex_match(synType,doubleQuotes)|| regex_match(synType,underScoreBothSides))
	{
		toReturn = synt_type::expression;
	}
	else if (regex_match(synType,underScoreOnly))
	{
		toReturn = synt_type::expression;
	}
	else if (regex_match(synType,integer))
	{
		toReturn = synt_type::integer;
	}
	else
	{
		if(newSymbol.exists(synType))
		{
			toReturn = newSymbol.getSyntType(synType);
		}
	}
	return toReturn;
}

query_type getType (char* queryType)
{
	if (strcmp(queryType,"Uses") == 0)
	{
		return query_type::uses;
	}
	else if (strcmp(queryType,"Modifies") == 0)
	{
		return query_type::modifies;
	}
	else if (strcmp(queryType,"Calls") == 0)
	{
		return query_type::calls;
	}
	else if (strcmp(queryType,"Calls*") == 0)
	{
		return query_type::callsStar;
	}
	else if (strcmp(queryType,"Parent") == 0)
	{
		return query_type::parent;
	}
	else if (strcmp(queryType,"Parent*") == 0)
	{
		return query_type::parentStar;
	}
	else if (strcmp(queryType,"Follows") == 0)
	{
		return query_type::follows;
	}
	else if (strcmp(queryType, "Follows*") == 0)
	{
		return query_type::followsStar;
	}
	else if (strcmp(queryType, "Next") == 0)
	{
		return query_type::next;
	}
	else if (strcmp(queryType, "Next*") == 0)
	{
		return query_type::nextStar;
	}
	else if (strcmp(queryType, "Affects") == 0)
	{
		return query_type::affects;
	}
	else if (strcmp(queryType, "Affects*") == 0)
	{
		return query_type::affectsStar;
	}
	else return query_type::error;
}

void removeCharsFromString( string &str, char* charsToRemove ) {
	for ( unsigned int i = 0; i < strlen(charsToRemove); ++i ) {
		str.erase( remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
	}
}

vector<string> split(const char *str, char c = ' ')
{
	vector<string> result;

	do
	{
		const char *begin = str;

		while(*str != c && *str)
			str++;

		result.push_back(string(begin, str));
	} while (0 != *str++);

	return result;
}

void ProcessEachToken(char *currentToken)
{
	if (strcmp(currentToken,"Select") == 0)
	{
		currToken = TOKEN::RESULT_CL;
	}
	else if (expectingThat && strcmp(currentToken, "that") == 0)
	{
		currToken = TOKEN::SUCHTHAT_CL;
	}
	else if (strcmp(currentToken,"with") == 0)
	{
		currToken = TOKEN::WITH_CL;
	}
	else if (strcmp(currentToken,"pattern") == 0)
	{
		if (currToken != TOKEN::RESULT_CL)
		{
			concatStmt = "";
		}
		currToken = TOKEN::PATTERN_CL;
	}
	else if (strcmp(currentToken,"such") == 0)
	{
		expectingThat = true;
	}

	else{

		switch(currToken)
		{

		case TOKEN::PATTERN_CL: 
			{  // assign a;
				// Select a pattern a(_, _"x + 1"_)
				patternExist = true;
				regex fullPattern("([^\\(]+\\(([^\\)]+|[^\,]+)(\,([^\\)]+|[^\,]+)\\))+)");

				concatStmt.append(currentToken);

				if (regex_match(concatStmt,fullPattern))
				{
					string supposedSynonym = "";
					synt_type patternSyn;
					string firstParameter = "";
					string secondParameter = "";
					string thirdParameter = "";

					bool inParameters = false;
					bool passedFirstParam = false;
					bool passedSecondParam = false;
					bool ifSyn = false;

					for(std::string::size_type i = 0; i < concatStmt.size(); ++i) {
						if (concatStmt[i] == '(')
						{
							// Begin getting the parameters
							bool isSynonymExist = newSymbol.exists(supposedSynonym);

							if (!isSynonymExist)
							{
								//TODO THROW ERROR because no such synonym in symbol table
								nonExistantSyn = true;
								break;
							}
							else
							{
								patternSyn = newSymbol.getSyntType(supposedSynonym);
								if (patternSyn != synt_type::assignment || patternSyn != synt_type::whileLoop || patternSyn != synt_type::ifelse )
								{
									// TODO THROW ERROR because pattern only accepts three types of synonyms
								}
								if (patternSyn == synt_type::ifelse)
								{
									ifSyn = true;
								}
							}
							inParameters = true;
						}
						else if (concatStmt[i] == ')')
						{
							concatStmt = "";
						}
						else if (inParameters && concatStmt[i] != ',')
						{
							if (!ifSyn)
							{
								if(passedFirstParam) 
								{
									secondParameter += concatStmt[i];
								}
								else {firstParameter += concatStmt[i]; }
							}
							else
							{
								if(!passedFirstParam)
								{
									firstParameter += concatStmt[i];
								}
								else if (!passedSecondParam)
								{
									secondParameter += concatStmt[i];
								}
								else 
								{
									thirdParameter += concatStmt[i];
								}

							}
						}
						else if (concatStmt[i] == ',')
						{
							if (!passedFirstParam)
							{
								passedFirstParam = true;
							}
							else
							{
								passedSecondParam = true;
							}
						}
						else if (!inParameters)
						{
							supposedSynonym += concatStmt[i]; 
						}
					}

					if (verifyCorrectParameters(patternSyn,firstParameter,secondParameter,thirdParameter))
					{
						ParamNode* leftParamNode = new ParamNode(patternSyn,supposedSynonym);
						synt_type middleParamSynType = getSynType(firstParameter);
						synt_type rightParamSynType = getSynType(secondParameter);
						ParamNode* middleParamNode = new ParamNode (middleParamSynType, firstParameter);
						ParamNode* rightParamNode = new ParamNode (rightParamSynType,secondParameter);
						PatternNode* newPattern = new PatternNode(leftParamNode,middleParamNode, rightParamNode);
						rootTree->addPattern(newPattern);
						patternPass = true;
					}
				}
				else
				{
					// TODO THROW ERROR because pattern is in the wrong syntax (doesn't match regex)
				}


			}
			break;
		case TOKEN::RESULT_CL: 
			{

				regex tuple("<.+>");
				regex boolean("BOOLEAN");
				// 
				if (regex_match(currentToken,tuple))
				{
					string strToken = currentToken;
					removeCharsFromString( strToken, "<>" );
					std::vector<char> writable(strToken.begin(), strToken.end());
					writable.push_back('\0');
					vector<string> variablesTuple = split(&writable[0],',');

					// do for loop for each variable to see if it exists in the symbol table;
					// code over here
					for(std::vector<string>::iterator i = variablesTuple.begin(); i != variablesTuple.end(); i++)
					{
						if (!newSymbol.exists(*i)) { tupleError = true; break;} // TODO THROW ERROR because variable doesn't exist
						else {newSymbol.setResult(newSymbol.getIndex(currentToken));}
					}
					concatStmt = "";

				}
				else if (regex_match(currentToken, boolean))
				{
					resultBool = true;
					concatStmt = "";
				}
				else
				{
					if (!newSymbol.exists(currentToken)) 
					{ 
						nonExistantSyn = true; 
						concatStmt = "";
						errorMsg += ("- Synonym \"%s\" after Select statement is not declared." , currentToken);
					} //TODO THROW ERROR because variable doesn't exist
					else
					{
						newSymbol.setResult(newSymbol.getIndex(currentToken));
					}
				}
			}break;

		case TOKEN::SUCHTHAT_CL: 
			{ // 
				regex fullPattern("([^\\(]+\\(([^\\)]+|[^\,]+)\,([^\\)]+|[^\,]+)\\))");
				suchThatQueryExist = true;

				concatStmt.append(currentToken);

				if (regex_match(concatStmt,fullPattern))
				{
					// means Modifies(..,...)
					string supposedRelationEntity = "";
					query_type queryClause;
					string firstParameter = "";
					string secondParameter = "";

					bool inParameters = false;
					bool passedFirstParam = false;

					for(std::string::size_type i = 0; i < concatStmt.size(); ++i) {
						if (concatStmt[i] == '(')
						{
							// Begin getting the parameters
							std::vector<char> writable(supposedRelationEntity.begin(), supposedRelationEntity.end());
							writable.push_back('\0');
							queryClause = getType(&writable[0]);
							if (queryClause == query_type::error)
							{
								//TODO THROW ERROR because no such such that clause (e.g. Modifies/Calls mispelled)
								break;
							}
							inParameters = true;
						}
						else if (concatStmt[i] == ')')
						{
							concatStmt = "";
						}
						else if (inParameters && concatStmt[i] != ',')
						{
							if(passedFirstParam) 
							{
								secondParameter += concatStmt[i];
							}
							else {firstParameter += concatStmt[i]; }
						}
						else if (concatStmt[i] == ',')
						{
							passedFirstParam = true;
						}
						else if (!inParameters)
						{
							supposedRelationEntity += concatStmt[i]; 
						}
					}
					// Now check if first param and second param are valid.
					// And create a querynode and add to the query tree
					if (verifyCorrectParameters(queryClause,firstParameter,secondParameter))
					{
						synt_type firstSyn = getSynType(firstParameter);
						synt_type secondSyn = getSynType(secondParameter);
						if (firstSyn != synt_type::synError && secondSyn != synt_type::synError)
						{
							ParamNode* firstParamNode = new ParamNode(firstSyn,firstParameter);
							ParamNode* secondParamNode = new ParamNode (secondSyn, secondParameter);
							QueryNode* newQuery = new QueryNode(queryClause,firstParamNode, secondParamNode);
							rootTree->addQuery(newQuery);
							suchThatQueryPass = true;
						}			

					}

				}
				// Else the such that clause doesn't match the regex, thus THROW ERROR
				else
				{
					//TODO THROW ERROR because the such that clause doesn't match the regex
				}

			}break;

		} // end of switch
	}

}

void Match (char *c)
{
	regex re(" *(procedure|stmtLst|stmt|assign|call|while|if|variable|constant|prog_line) +([a-zA-Z])+(\\d|#)*( *, *[a-zA-Z]+(\\d|#)*)*");
	regex sel(" *Select *.*");

	if (regex_match(c,re))
	{
		int count =0;
		char *pch;
		char *end_token;
		pch = strtok_s (c," ,",&end_token);
		bool entityTypeFound = false;
		synt_type newSyntType = synt_type::synError;

		while (pch != NULL)
		{
			//printf ("Split design entity ',' : %s\n",pch);

			if (!entityTypeFound)
			{
				if (strcmp(pch,"while") == 0)
				{
					newSyntType= synt_type::whileLoop;
				}
				else if (strcmp(pch,"assign") == 0)
				{
					newSyntType = synt_type::assignment;
				}
				else if (strcmp(pch,"procedure") == 0)
				{
					newSyntType = synt_type::procedure;
				}
				else if (strcmp(pch,"stmtLst") == 0)
				{
					newSyntType = synt_type::statementList;
				}
				else if (strcmp(pch,"stmt") == 0 )
				{
					newSyntType = synt_type::statement;
				}
				else if (strcmp(pch,"call") == 0)
				{
					newSyntType = synt_type::call;
				}
				else if (strcmp(pch,"if") == 0)
				{
					newSyntType = synt_type::ifelse;
				}
				else if (strcmp(pch,"variable") == 0)
				{
					newSyntType = synt_type::variable;
				}
				else if (strcmp(pch,"constant") == 0)
				{
					newSyntType = synt_type::constant;
				}
				else if (strcmp(pch,"prog_line") == 0)
				{
					newSyntType = synt_type::progline;
				}
				entityTypeFound = true;
			}
			else
			{
				++count;
				// store each variable in the symbol table
				newSymbol.setVar(pch,newSyntType);

			}
			pch = strtok_s (NULL, " ,",&end_token);
		}
		--count;
		// count number of variables
	}
	else if (regex_match(c,sel))
	{
		char *pch;
		char *end_token;
		pch = strtok_s (c," ",&end_token);

		while (pch != NULL)
		{
			//printf ("For each word in select statement : %s\n",pch);
			ProcessEachToken(pch);
			pch = strtok_s (NULL, " ",&end_token);
		}
	}
	else
	{
		errorMsg += "Syntax error in synonym declaration!;\n";
	}

}


void readQuery(std::string i)
{
	const char* s = ";";
	char *end_str;
	char *token;

	rootTree = new QueryTree();

	expectingThat = false;
	resultBool = false;
	tupleError = false;
	nonExistantSyn = false;
	suchThatQueryExist = false;
	patternExist = false;
	suchThatQueryPass = false;
	patternPass = false;

	char *a=new char[i.size()+1];
	a[i.size()]=0;
	memcpy(a,i.c_str(),i.size());

	token = strtok_s(a,s,&end_str);

	/* walk through other tokens */

	while( token != NULL ) 
	{
		Match(token);
		token = strtok_s(NULL, s,&end_str);
		if (tupleError)
		{
			break;
		}
	}
	if ((suchThatQueryPass == false && suchThatQueryExist) || (patternPass == false && patternExist))
	{
		//Throw error for query part
		cout << errorMsg;
	}
	else // means there were no errors
	{
		printf("QUERY TREE DONE. NOW TO EVALUATE");
		//QueryEvaluator evaluator (&newSymbol,rootTree);
		system("pause");

	}

}
void readSourceFile(std::string sourceFile){

	const char* s = ";";
	char *end_str;
	char *token;

	string i ;

	ifstream infile;
	infile.open (sourceFile);

	while(!infile.eof()) // To get you all the lines.
	{
		getline(infile,i); // Saves the line in STRING.
		rootTree = new QueryTree();
		expectingThat = false;
		resultBool = false;
		tupleError = false;
		nonExistantSyn = false;
		suchThatQueryExist = false;
		patternExist = false;
		suchThatQueryPass = false;
		patternPass = false;
		char *a=new char[i.size()+1];
		a[i.size()]=0;
		memcpy(a,i.c_str(),i.size());

		token = strtok_s(a,s,&end_str);

		/* walk through other tokens */

		while( token != NULL ) 
		{
			Match(token);
			token = strtok_s(NULL, s,&end_str);
			if (tupleError)
			{
				break;
			}
		}
		if ((suchThatQueryPass == false && suchThatQueryExist) || (patternPass == false && patternExist))
		{
			//Throw error for query part
			cout << errorMsg;
		}
		else // means there were no errors
		{
			printf("QUERY TREE DONE. NOW TO EVALUATE");
			//QueryEvaluator evaluator (&newSymbol,rootTree);

		}
	}
	infile.close();
	system ("pause");
}

// Start here
int main()
{
	string sourceFile;
	getline(cin,sourceFile);
	readQuery(sourceFile);

	//cin >>  sourceFile;
	//readSourceFile(sourceFile);

	return 0;
}

void print( vector <string> & v )
{
	for (size_t n = 0; n < v.size(); n++)
		cout << "\"" << v[ n ] << "\"\n";
	cout << endl;
}
