#include <iostream>
#include <string>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <regex>

using namespace std;

enum TOKEN { SELECT, RESULT_CL, WITH_CL, SUCHTHAT_CL, PATTERN_CL };

TOKEN currToken = TOKEN::SELECT;

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
	else{
		if (strcmp(currentToken,"with") == 0)
		{
			currToken = TOKEN::WITH_CL;
		}
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

					// do for loop for each variable to see if it exists;
					// code over here


				}
				else if (regex_match(currentToken, boolean))
				{

				}
				else
				{

				}
			}break;
		}
	}

}

void Match (char *c)
{
	regex re(" *(while|assign) +([a-zA-Z])+(\\d|#)*( *, *[a-zA-Z]+(\\d|#)*)*");
	regex sel(" *select *.*");

	if (regex_match(c,re))
	{
		int count =0;
		char *pch;
		char *end_token;
		printf ("Splitting string \"%s\" into tokens:\n",c);
		pch = strtok_s (c," ,",&end_token);
		bool entityTypeFound = false;

		while (pch != NULL)
		{
			printf ("%s\n",pch);
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
		printf ("Splitting string \"%s\" into tokens:\n",c);
		pch = strtok_s (c," ",&end_token);

		while (pch != NULL)
		{
			printf ("%s\n",pch);
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
		printf( "TOKEN: %s\n", token );
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
