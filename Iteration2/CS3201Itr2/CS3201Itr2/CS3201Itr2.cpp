// CS3201Itr2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SPA.h"


int _tmain(int argc, _TCHAR* argv[])
{
	SPA* spa = new SPA();
	std::string file;
	std::cin>>file;
	spa->processSimpleSourceFile(file);
	spa->startCommandLinePQLShell();
}

