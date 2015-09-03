#include <iostream>
#include <string>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <regex>


using namespace std;

enum TOKEN { DESGENITY, SYNONYM, ERROR };
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
	for (int a = 0; a < _countof(designEntity); a ++)
	{
		if (strcmp(c,designEntity[a]) == 0)
		{
			//printf("FIRST\n");
			//printf( "%s\n", c );
			currentToken = TOKEN::SYNONYM;
			foundEntity = true;
		}
	}
	if (!foundEntity)
	{
		//error
		currentToken = TOKEN::ERROR;
	}
		}
	break;

	case SYNONYM :{
		//printf("NEXT\n");printf( "%s\n", c );
				  string s = c;
				  smatch m; 
				  regex e ("([a-zA-Z])+(\d|#)*");

				    while (regex_search (s,m,e)) {
for (auto i = s.begin(); i != s.end(); ++i)
{
    s = m.suffix().str();
	//printf( "CORRECT: %s\n", s );
}
    
  }


				  }
		break;
	}
}



void GetToken()
{

}

int main()
{
	//cout << "Hello World!";
	//char str[80] = "Select s from something";
	const char* s = " ";
	char* token;
	TOKEN currentToken = DESGENITY;

	string i ;
	getline (cin, i);
	char *a=new char[i.size()+1];
	a[i.size()]=0;
	memcpy(a,i.c_str(),i.size());
	char car; 

	token = strtok(a,s);
	bool foundID = false;
	bool foundSelect = false;

	/* get the first token */
   //token = strtok(str, s);
   
   /* walk through other tokens */
   while( token != NULL ) 
   {
      //printf( " %s\n", token );
	  char c = *token;
	  if (isdigit(c))
	  {
		  foundID = true;
		  //printf( " %s ii\n", token );
	  }
	  Match(token);
      token = strtok(NULL, s);
	  
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
