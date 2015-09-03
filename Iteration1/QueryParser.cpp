#include <iostream>
#include <string>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <regex>


using namespace std;

enum TOKEN { DESGENITY, SYNONYM, SELECT, ERROR };
TOKEN currentToken = TOKEN::DESGENITY;

class QueryTree
{
	int ID;
public:
	void setID(int);

};
void QueryTree::setID(int id)
{
	ID = id;
}

void Match (char *c)
{
	TOKEN curr = currentToken;
	switch (curr)
	{
	case DESGENITY : 
		{
			bool foundEntity = false;
			static const char * const designEntity[] = {"procedure", "stmtLst" , "stmt", "assign", "call", "while", "if", "variable", "constant", "prog_line"};

			regex re(" *(while|assign) +([a-zA-Z])+(\\d|#)*( *, *[a-zA-Z]+(\\d|#)*)*");
			smatch match;
			string result = " ";
			//string s = c;

			if (regex_match(c,re))
			{
				int count =0;
				char *pch;
				char *end_token;
				printf ("Splitting string \"%s\" into tokens:\n",c);
				pch = strtok_s (c," ,",&end_token);
				while (pch != NULL)
				{
					printf ("%s\n",pch);
					pch = strtok_s (NULL, " ,",&end_token);
					++count;
				}
				--count;
				printf("count :");
			}

			/*
			if (regex_search(s, match, re) && match.size() > 1) {
			result = match.str(1);
			printf( "HERE\n");
			for (smatch::iterator it = match.begin(); it!=match.end(); ++it) {
			std::cout << *it << std::endl;
			}
			} 
			else {
			result = string("WRONG");
			} 


			sregex_iterator next(s.begin(), s.end(), re) ;sregex_iterator end;
			while (next != end) {
			match = *next;
			printf( "HERE\n");
			std::cout << match.str() << "\n";
			next++;

			}
			*/
		}
		break;

	case SYNONYM :{
		printf("NEXT\n");printf( "%s\n", c );
		string s = c;
		string result = " ";
		smatch m; 
		regex re ("([a-zA-Z])+(\d|#)*");
		regex semiColon(".*;$");

		smatch match;
		if (regex_search(s, match, re) && match.size() > 1) {
			result = match.str(1);
			//printf( "CORRECT: %s\n", result );
		} 
		else {
			result = string("WRONG");
		} 
				  }

				  break;
	case SELECT: break;
	}
}



void GetToken()
{

}

int main()
{
	//cout << "Hello World!";
	//char str[80] = "Select s from something";
	const char* s = ";";
	char *end_str;
	char *token;
	TOKEN currentToken = DESGENITY;

	string i ;
	getline (cin, i);
	char *a=new char[i.size()+1];
	a[i.size()]=0;
	memcpy(a,i.c_str(),i.size());


	token = strtok_s(a,s,&end_str);

	/* walk through other tokens */

	// while x,y
	// assign a
	// select a

	while( token != NULL ) 
	{
		printf( "TOKEN: %s\n", token );
		//char c = *token;
		/*if (isdigit(c))
		{
		foundID = true;
		//printf( " %s ii\n", token );
		}
		*/

		/*
		string delimiter = " ";
		string newToken = token;
		size_t pos = 0;
		string tokenu;
		while ((pos = newToken.find(delimiter)) != std::string::npos) {
		tokenu = newToken.substr(0, pos);
		std::cout << tokenu << std::endl;
		newToken.erase(0, pos + delimiter.length());
		}
		std::cout << s << std::endl;


		char str[] ="- This, a sample string.";
		char * pch;
		printf ("Splitting string \"%s\" into tokens:\n",str);
		pch = strtok (str," ,.-");
		while (pch != NULL)
		{
		printf ("%s\n",pch);
		pch = strtok (NULL, " ");
		}
		*/
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
