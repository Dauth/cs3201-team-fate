#include "stdafx.h"
#include "SynonymValues.h"

SynonymValues::SynonymValues(string s) {
	name = s;
}

string SynonymValues::getName() {
	return name;
}

set<string> SynonymValues::getValues() {
	return values;
}

void SynonymValues::setValues(set<string> set) {
	values = set;
}