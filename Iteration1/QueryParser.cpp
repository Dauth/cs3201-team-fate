#include <iostream>
#include <string>

using namespace std;


class QueryParser{
	/*validation parser*/
	/* handle of declarations : check for variables and put into symbol table*/
	/*Then handle the queries*/

	//method to add to the querytree
	//method to add to the symbol table

void Match (char *c)
{
	printf( " %s\n", c );
}
void GetNextToken()
{

}

int main()
{
	//cout << "Hello World!";
	//char str[80] = "Select s from something";
	const char* s = " ";
	char* token;

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
      printf( " %s\n", token );
	  char c = *token;
	  if (isdigit(c))
	  {
		  foundID = true;
		  printf( " %s ii\n", token );
	  }
	  Match(token);
      token = strtok(NULL, s);
	  
   }
   int tt;
   cin >>  tt;

	
	return 0;
}

}


