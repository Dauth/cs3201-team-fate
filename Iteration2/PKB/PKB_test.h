#ifndef PKB_TEST_H
#define PKB_TEST_H

#include "PKB.h"

class PKB_test {
	PKB* pkb;

public:
	PKB_test();
	void setupTestData();
	void runTests();
	void printResults(vector<pair<string, string>>);

};


#endif