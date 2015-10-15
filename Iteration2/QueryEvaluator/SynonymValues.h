#ifndef SYNONYMVALUES_H
#define SYNONYMVALUES_H

#include "stdafx.h"

using namespace std;

class SynonymValues {
	string name;
	set<string> values;

public:
	SynonymValues(string);
	string getName();
	set<string> getValues();

	void setValues(set<string>);
};

#endif