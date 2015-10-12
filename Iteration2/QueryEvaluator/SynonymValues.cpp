#include "stdafx.h"
#include "SynonymValues.h"

SynonymValues::SynonymValues(std::string s) {
	name = s;
}

std::string SynonymValues::getName() {
	return name;
}

std::set<std::string> SynonymValues::getValues() {
	return values;
}

void SynonymValues::setValues(std::set<std::string> set) {
	values = set;
}