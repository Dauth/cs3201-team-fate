#ifndef SYNONYMVALUES_H
#define SYNONYMVALUES_H

#include "stdafx.h"

class SynonymValues {
	std::string name;
	std::set<std::string> values;

public:
	SynonymValues(std::string);
	std::string getName();
	std::set<std::string> getValues();

	void setValues(std::set<std::string>);
};

#endif