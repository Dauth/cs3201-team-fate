#include <iostream>
#include <string>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <regex>

using namespace std;

enum TOKEN { SELECT, RESULT_CL, WITH_CL, SUCHTHAT_CL, PATTERN_CL };

TOKEN currToken = TOKEN::SELECT;
bool expectingThat = false;
string concatSuchThatStmt = "";

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
	if (strcmp(currentToken,"select") == 0)
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
		case RESULT_CL: 
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


				}
				else if (regex_match(currentToken, boolean))
				{

				}
				else
				{

				}
			}break;

		case SUCHTHAT_CL: 
			{ // 
				regex stmtRef("(Parent|Parent\\*|Affects|Affects\\*|Follows|Follows\\*)\\((([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+),(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)\\)");
				regex entRef();
				regex varRef();
				concatSuchThatStmt.append(currentToken);
				if (regex_match(concatSuchThatStmt,stmtRef))
				{
					// means Modifies(..,...)
				}
				printf("%s \n", currentToken);
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

		while (pch != NULL)
		{
			printf ("Split design entity ',' : %s\n",pch);
			pch = strtok_s (NULL, " ,",&end_token);
			if (!entityTypeFound)
			{
				entityTypeFound = true;
			}
			else
			{
				++count;
				// store each variable in the symbol table
			}
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
