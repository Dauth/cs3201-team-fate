#ifndef PKB_TEST_H
#define PKB_TEST_H

#include "PKB.h"

class PKB_test {
	PKB* pkb;

public:
	PKB_test();
	void setupTestData();
	void runTests();
	void testModifiesProc();
	void testModifiesStmt();
	void testModifiesAssign();
	void testModifiesWhile();
	void testModifiedBySpecifiedVar();
	void testModifiedByVarType();
	void testModifiedByNested();
	void testUsesProc();
	void testUsesStmt();
	void testUsesAssign();
	void testUsesWhile();
	void testUsedBySpecificVar();
	void testUsedByVarType();
	void testParentStatement();
	void testParentWhile() ;
	void testChildStatement();
	void testChildAssign();
	void testFollowsStatement();
	void testFollowsWhile();
	void testFollowedByStatement();
	void testFollowedByAssignment();
	void printResults(vector<pair<string, string>>);

};


#endif