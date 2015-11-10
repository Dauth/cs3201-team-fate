#ifndef PKB_TEST_H
#define PKB_TEST_H

#include "PKBFacade.h"

class PKB_test {
	PKBFacade* pkb;

public:
	PKB_test();
	void setupTestData();
	void runTests();
	void printResults(vector<pair<string, string>>);

};


#endif