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

using namespace std;

enum TOKEN { SELECT, RESULT_CL, WITH_CL, SUCHTHAT_CL, PATTERN_CL };

TOKEN currToken = TOKEN::SELECT;
bool expectingThat = false;
string concatStmt = "";
bool resultBool = false;
QueryTree rootTree;
Symbol newSymbol;
bool suchThatQueryPass = false;
const string errorMsg = "ERROR : %s";

//regex stmtRef_old("(Parent|Parent\\*|Affects|Affects\\*|Follows|Follows\\*)\\((([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+),(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)\\)");

// INDENT | "_" | INTEGER
regex stmtRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)");
regex lineRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)");

// INDENT | "_"| "INDENT" | INTEGER
regex entRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+|\"([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*\")");

// INDENT | "_" | "INDENT"
regex varRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|\"([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*\")");





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
	regex doubleQuotes ("\"[^\"]\"");
	regex integer ("\\d+");
	synt_type toReturn = synt_type::errorr;

	if (regex_match(synType,doubleQuotes))
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

void GetToken(char *currentToken)
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
		currToken = TOKEN::PATTERN_CL;
	}
	else if (strcmp(currentToken,"such") == 0)
	{
		expectingThat = true;
	}

	else{

		switch(currToken)
		{

		case PATTERN_CL: 
			{  // assign a;
				// Select a pattern a(_, _"x + 1"_)
				
				regex fullPattern("([^\\(]+\\(([^\\)]+|[^\,]+),([^\\)]+|[^\,]+)\\))");

				concatStmt.append(currentToken);

				if (regex_match(concatStmt,fullPattern))
				{
					string supposedSynonym = "";
					string firstParameter = "";
					string secondParameter = "";

					bool inParameters = false;
					bool passedFirstParam = false;

					for(std::string::size_type i = 0; i < concatStmt.size(); ++i) {
						if (concatStmt[i] == '(')
						{
							// Begin getting the parameters
							bool isSynonymExist = newSymbol.exists(supposedSynonym);

							if (!isSynonymExist)
							{
								//TODO THROW ERROR because no such synonym in symbol table
								break;
							}
							inParameters = true;
						}
						else if (concatStmt[i] == ')')
						{
							// Do nothing.
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
							supposedSynonym += concatStmt[i]; 
						}
					}


				}
				else
				{
					// TODO THROW ERROR because pattern is in the wrong syntax
				}
				
				
			}
			break;
		case RESULT_CL: 
			{
				if(!concatStmt.empty())
				{
					concatStmt == "";
				}

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
						if (!newSymbol.exists(*i)) { break;} // TODO THROW ERROR because variable doesn't exist
					}

				}
				else if (regex_match(currentToken, boolean))
				{
					resultBool = true;
				}
				else
				{
					if (!newSymbol.exists(currentToken)) { } //TODO THROW ERROR because variable doesn't exist
				}
			}break;

		case SUCHTHAT_CL: 
			{ // 
				regex fullPattern("([^\\(]+\\(([^\\)]+|[^\,]+),([^\\)]+|[^\,]+)\\))");
				if(!concatStmt.empty())
				{
					concatStmt == "";
				}

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
							// Do nothing.
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
						if (firstSyn != synt_type::errorr && secondSyn != synt_type::errorr)
						{
							ParamNode* firstParamNode = new ParamNode(firstSyn,firstParameter);
							ParamNode* secondParamNode = new ParamNode (secondSyn, secondParameter);
							QueryNode* newQuery = new QueryNode(queryClause,firstParamNode, secondParamNode);
							rootTree.addQuery(newQuery);
						}			

					}
					suchThatQueryPass = true;
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
		synt_type newSyntType = synt_type::errorr;

		while (pch != NULL)
		{
			printf ("Split design entity ',' : %s\n",pch);

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
			printf ("For each word in select statement : %s\n",pch);
			GetToken(pch);
			pch = strtok_s (NULL, " ",&end_token);
		}
	}

}

int main()
{
	const char* s = ";";
	char *end_str;
	char *token;

	string i ;
	getline (cin, i);
	char *a=new char[i.size()+1];
	a[i.size()]=0;
	memcpy(a,i.c_str(),i.size());

	token = strtok_s(a,s,&end_str);

	/* walk through other tokens */

	while( token != NULL ) 
	{
		Match(token);
		token = strtok_s(NULL, s,&end_str);

	}
	if (!suchThatQueryPass)
	{
		//Throw error for query part
	}
	int tt;
	cin >>  tt;


	return 0;
}


void print( vector <string> & v )
{
	for (size_t n = 0; n < v.size(); n++)
		cout << "\"" << v[ n ] << "\"\n";
	cout << endl;
}
