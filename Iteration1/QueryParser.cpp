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

regex stmtRef("(Parent|Parent\\*|Affects|Affects\\*|Follows|Follows\\*)\\((([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+),(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)\\)");

// INDENT | "_"| "INDENT" | INTEGER
regex entRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+|\"([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*\")");

// INDENT | "_" | "INDENT"
regex varRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|\"([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*\")");



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
				smatch m;
				string stringToken = currentToken;
				string result = "";
				regex patternCond("([^\\(]+\\()");
				regex fullPattern("(\\([^\\)]+\\))");
				/*
				if(!concatStmt.empty())
				{
				concatStmt = "";
				}
				*/
				concatStmt.append(currentToken);
				// now verify with symbol table
				if (regex_search(concatStmt,m,patternCond))
				{
					result = m.str(1);
					removeCharsFromString( result, "(" );

					// verify with symbol table
					if (newSymbol.exists(result))
					{
						char *toBeRemoved = &result[0];
						removeCharsFromString( concatStmt, toBeRemoved ); // now get the parameters
					}
					else 
					{
						//TODO THROW ERROR because variable doesn't exist in symbol table
					}
				}
				if (regex_match(concatStmt,fullPattern))
				{
					removeCharsFromString( concatStmt, "()" );
					std::vector<char> writable(concatStmt.begin(), concatStmt.end());
					writable.push_back('\0');
					vector<string> patternParam = split(&writable[0],',');
					for(std::vector<string>::iterator i = patternParam.begin(); i != patternParam.end(); i++)
					{
						cout << *i;
					}
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
				regex fullPattern("([^\\(]+\\([^\\)]+\\))");
				if(!concatStmt.empty())
				{
					concatStmt == "";
				}

				concatStmt.append(currentToken);

				if (regex_match(concatStmt,fullPattern))
				{
					// means Modifies(..,...)
					string supposedRelationEntity = "";
					string firstParameter = "";
					string secondParameter = "";

					bool inParameters = false;

					for(std::string::size_type i = 0; i < concatStmt.size(); ++i) {
						if (concatStmt[i] == '(')
						{
							// Begin getting the parameters
							std::vector<char> writable(supposedRelationEntity.begin(), supposedRelationEntity.end());
							writable.push_back('\0');
							getType(&writable[0]);
							bool inParameters = true;
						}
						else if (inParameters)
						{

						}
						else if (concatStmt[i] == ',')
						{

						}
						else if (!inParameters)
						{
							supposedRelationEntity += concatStmt[i]; 
						}
					}
					printf("%s herehere \n", supposedRelationEntity);
				}
				
			}break;

		} // end of switch
	}

}

query_type getType (char* queryType)
{
	if (strcmp(queryType,"Uses"))
	{
		return query_type::uses;
	}
	else if (strcmp(queryType,"Modifies"))
	{
		return query_type::modifies;
	}
	else if (strcmp(queryType,"Calls"))
	{
		return query_type::calls;
	}
	else if (strcmp(queryType,"Calls*"))
	{
		return query_type::callsStar;
	}
	else if (strcmp(queryType,"Parent"))
	{
		return query_type::parent;
	}
	else if (strcmp(queryType,"Parent*"))
	{
		return query_type::parentStar;
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
		synt_type newSyntType = synt_type::variable;

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
				newSymbol.setVar(pch,newSyntType);
				// store each variable in the symbol table
			}
			pch = strtok_s (NULL, " ,",&end_token);
		}
		--count;
		// count number of variables
	}
	else if (regex_match(c,sel))
	{
		printf ("Select st");
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
	//cout << "Hello World!";
	//char str[80] = "Select s from something";
	const char* s = ";";
	char *end_str;
	char *token;
	//TOKEN currentToken = DESGENITY;

	string i ;
	getline (cin, i);
	char *a=new char[i.size()+1];
	a[i.size()]=0;
	memcpy(a,i.c_str(),i.size());

	token = strtok_s(a,s,&end_str);

	/* walk through other tokens */

	while( token != NULL ) 
	{
		printf( "First split by ';' : %s\n", token );
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
