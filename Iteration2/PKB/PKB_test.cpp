// spa_build.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PKB_test.h"

using namespace std;

PKB_test::PKB_test() {
	ExpressionTree* et = new ExpressionTree();
	pkb = new PKB(et);
}


void PKB_test::setupTestData() {
	// procedure test {
	Node* proc = pkb->createProcedure("test");
	Node* testStmtLst1 = pkb->createNode(statementList, 0);
	proc->setLeftChild(testStmtLst1);

	// 1. y = 3
	Node* s1 = pkb->createNode(assignment, 1, "", nullptr, nullptr, nullptr, proc);
	Node* s1a = pkb->createNode(variable, 1, "y", nullptr, s1, nullptr, proc);
	Node* s1c3 = pkb->createNode(constant, 1, "3");
	s1->setLeftChild(s1a);
	s1->setRightChild(s1c3);
	testStmtLst1->addStmt(s1);

	// 2. x = 4
	Node* s2 = pkb->createNode(assignment, 2, "", nullptr, nullptr, nullptr, proc);
	Node* s2a = pkb->createNode(variable, 2, "x", nullptr, s2, nullptr, proc);
	Node* s2c4 = pkb->createNode(constant, 2, "4");
	s2->setLeftChild(s2a);
	s2->setRightChild(s2c4);
	testStmtLst1->addStmt(s2);

	// 3. z = x + y
	Node* s3 = pkb->createNode(assignment, 3, "", nullptr, nullptr, nullptr, proc);
	Node* s3a = pkb->createNode(variable, 3, "z", nullptr, s3, nullptr, proc);
	Node* s3v_x = pkb->createNode(variable, 3, "x", s3, nullptr, nullptr, proc);
	Node* s3e_plus = pkb->createNode(expression, 3, "+", s3, nullptr, nullptr, proc);
	Node* s3v_y = pkb->createNode(variable, 3, "y", s3, nullptr, nullptr, proc);
	s3->setLeftChild(s3a);
	s3->setRightChild(s3e_plus);
	s3e_plus->setLeftChild(s3v_x);
	s3e_plus->setRightChild(s3v_y);
	testStmtLst1->addStmt(s3);

	// 4. While x {
	Node* s4 = pkb->createNode(whileLoop, 4, "", nullptr, nullptr, nullptr, proc);
	Node* s4v_x = pkb->createNode(variable, 4, "x", s4, nullptr, nullptr, proc);
	Node* whileStmtLst = pkb->createNode(statementList, 4);
	s4->setLeftChild(s4v_x);
	s4->setRightChild(whileStmtLst);
	testStmtLst1->addStmt(s4);

	// 5. a = z + y + x
	Node* s5 = pkb->createNode(assignment, 5, "", nullptr, nullptr, s4, proc);
	Node* s5a = pkb->createNode(variable, 5, "a", nullptr, s5, s4, proc);
	Node* s5v_z = pkb->createNode(variable, 5, "z", s5, nullptr, s4, proc);
	Node* s5e_plus1 = pkb->createNode(expression, 5, "+", s5, nullptr, nullptr, proc);
	Node* s5v_y = pkb->createNode(variable, 5, "y", s5, nullptr, s4, proc);
	Node* s5e_plus2 = pkb->createNode(expression, 5, "+", s5, nullptr, nullptr, proc);
	Node* s5v_x = pkb->createNode(variable, 5, "x", s5, nullptr, s4, proc);
	s5->setLeftChild(s5a);
	s5->setRightChild(s5e_plus1);
	s5e_plus1->setLeftChild(s5e_plus2);
	s5e_plus1->setRightChild(s5v_x);
	s5e_plus2->setLeftChild(s5v_z);
	s5e_plus2->setRightChild(s5v_y);
	whileStmtLst->addStmt(s5);

	// 6. b = a + z
	Node* s6 = pkb->createNode(assignment, 6, "", nullptr, nullptr, s4, proc);
	Node* s6a = pkb->createNode(variable, 6, "b", nullptr, s6, s4, proc);
	Node* s6v_a = pkb->createNode(variable, 6, "a", s6, nullptr, s4, proc);
	Node* s6e_plus = pkb->createNode(expression, 6, "+", s6, nullptr, nullptr, proc);
	Node* s6v_z = pkb->createNode(variable, 6, "z", s6, nullptr, s4, proc);
	s6->setLeftChild(s6a);
	s6->setRightChild(s6e_plus);
	s6e_plus->setLeftChild(s6v_a);
	s6e_plus->setRightChild(s6v_z);
	whileStmtLst->addStmt(s6);

	// 7. While y {
	Node* s7 = pkb->createNode(whileLoop, 7, "", nullptr, nullptr, s4, proc);
	Node* s7v_x = pkb->createNode(variable, 7, "y", s7, nullptr, s4, proc);
	Node* whileStmtLst2 = pkb->createNode(statementList, 7);
	s7->setLeftChild(s7v_x);
	s7->setRightChild(whileStmtLst2);
	whileStmtLst->addStmt(s7);


	// 8. t = a + x }
	Node* s8 = pkb->createNode(assignment, 8, "", nullptr, nullptr, s7, proc);
	Node* s8a = pkb->createNode(variable, 8, "t", nullptr, s8, s7, proc);
	Node* s8v_a = pkb->createNode(variable, 8, "a", s8, nullptr, s7, proc);
	Node* s8e_plus = pkb->createNode(expression, 8, "+", s8, nullptr, nullptr, proc);
	Node* s8v_z = pkb->createNode(variable, 8, "x", s8, nullptr, s7, proc);
	s8->setLeftChild(s8a);
	s8->setRightChild(s8e_plus);
	s8e_plus->setLeftChild(s8v_a);
	s8e_plus->setRightChild(s8v_z);
	whileStmtLst2->addStmt(s8);

	// 9. c = z + x }
	Node* s9 = pkb->createNode(assignment, 9, "", nullptr, nullptr, s4, proc);
	Node* s9a = pkb->createNode(variable, 9, "c", nullptr, s9, s4, proc);
	Node* s9v_z = pkb->createNode(variable, 9, "z", s9, nullptr, s4, proc);
	Node* s9e_plus = pkb->createNode(expression, 9, "+", s9, nullptr, nullptr, proc);
	Node* s9v_x = pkb->createNode(variable, 9, "x", s9, nullptr, s4, proc);
	s9->setLeftChild(s9a);
	s9->setRightChild(s9e_plus);
	s9e_plus->setLeftChild(s9v_z);
	s9e_plus->setRightChild(s9v_x);
	whileStmtLst->addStmt(s9);

	// 10. x = x + z
	Node* s10 = pkb->createNode(assignment, 10, "", nullptr, nullptr, nullptr, proc);
	Node* s10a = pkb->createNode(variable, 10, "x", nullptr, s10, nullptr, proc);
	Node* s10v_x = pkb->createNode(variable, 10, "x", s10, nullptr, nullptr, proc);
	Node* s10e_plus = pkb->createNode(expression, 10, "+", s10, nullptr, nullptr, proc);
	Node* s10v_z = pkb->createNode(variable, 10, "z", s10, nullptr, nullptr, proc);
	s10->setLeftChild(s10a);
	s10->setRightChild(s10e_plus);
	s10e_plus->setLeftChild(s10v_x);
	s10e_plus->setRightChild(s10v_z);
	testStmtLst1->addStmt(s10);

	// 11. a = 3 + y
	Node* s11 = pkb->createNode(assignment, 11, "", nullptr, nullptr, nullptr, proc);
	Node* s11a = pkb->createNode(variable, 11, "a", nullptr, s11, nullptr, proc);
	Node* s11c_3 = pkb->createNode(constant, 11, "3");
	Node* s11e_plus = pkb->createNode(expression, 11, "+", s11, nullptr, nullptr, proc);
	Node* s11v_z = pkb->createNode(variable, 11, "y", s11, nullptr, nullptr, proc);
	s11->setLeftChild(s11a);
	s11->setRightChild(s11e_plus);
	s11e_plus->setLeftChild(s11c_3);
	s11e_plus->setRightChild(s11v_z);
	testStmtLst1->addStmt(s11);

	// 12. call test2 }
	Node* s12 = pkb->createNode(call, 12, "test2", nullptr, nullptr, nullptr, proc);
	testStmtLst1->addStmt(s12);

	// procedure test3 {
	Node* proc3 = pkb->createProcedure("test3");
	Node* test3StmtLst = pkb->createNode(statementList, 0);
	proc->setLeftChild(test3StmtLst);

	// 13. f = g + h }
	Node* s13 = pkb->createNode(assignment, 13, "", nullptr, nullptr, nullptr, proc3);
	Node* s13a = pkb->createNode(variable, 13, "f", nullptr, s13, nullptr, proc3);
	Node* s13v_g = pkb->createNode(variable, 13, "g", s13, nullptr, nullptr, proc3);
	Node* s13e_plus = pkb->createNode(expression, 13, "+", s13, nullptr, nullptr, proc3);
	Node* s13v_h = pkb->createNode(variable, 13, "h", s13, nullptr, nullptr, proc3);
	s13->setLeftChild(s13a);
	s13->setRightChild(s13e_plus);
	s13e_plus->setLeftChild(s13v_g);
	s13e_plus->setRightChild(s13v_h);
	test3StmtLst->addStmt(s13);

	// procedure test2 {
	Node* proc2 = pkb->createProcedure("test2");
	Node* test2StmtLst = pkb->createNode(statementList, 0);
	proc->setLeftChild(test2StmtLst);

	// 14. i = j + k 
	Node* s14 = pkb->createNode(assignment, 14, "", nullptr, nullptr, nullptr, proc2);
	Node* s14a = pkb->createNode(variable, 14, "i", nullptr, s14, nullptr, proc2);
	Node* s14v_x = pkb->createNode(variable, 14, "j", s14, nullptr, nullptr, proc2);
	Node* s14e_plus = pkb->createNode(expression, 14, "+", s14, nullptr, nullptr, proc2);
	Node* s14v_y = pkb->createNode(variable, 14, "k", s14, nullptr, nullptr, proc2);
	s14->setLeftChild(s14a);
	s14->setRightChild(s14e_plus);
	s14e_plus->setLeftChild(s14v_x);
	s14e_plus->setRightChild(s14v_y);
	test2StmtLst->addStmt(s14);

	// 15. call test3 }
	Node* s15 = pkb->createNode(call, 15, "test3", nullptr, nullptr, nullptr, proc2);
	test2StmtLst->addStmt(s15);
}

void PKB_test::printResults(vector<pair<string, string>> results) {
	cout << "Obtained " << results.size() << " results\n";
	for(int i=0; i<results.size(); i++) {
		cout << "( " + results[i].first + ", " + results[i].second + " ) ";
	}
}

void PKB_test::runTests() {

	//vector<string> r = sp->readSourceFile("C:\\Users\\user\\Documents\\simple.txt");
	//vector<Node*> a = sp->execute(r);
	// procedure test {
	// 1. y = 3
	// 2. x = 4
	// 3. z = x + y
	// 4. While x {
	// 5.	a = z + y + x
	// 6.	b = a + z
	// 7.	While y {
	// 8.		t = a + x }
	// 9.	c = z + x }
	// 10. x = x + z
	// 11. a = 3 + y
	// 12. call test2 }
	// procedure test3 {
	// 13. f = g + h }
	// procedure test2 {
	// 14. i = j + k
	// 14. call test3 }
	
	setupTestData();

	cout << "\n\nQuery is Modifies(\"test\", variable) \n"; 
	printResults(pkb->getModifies("test", variable));

	cout << "\n\nQuery is Modifies(\"5\", variable) \n"; 
	printResults(pkb->getModifies("5", variable));

	cout << "\n\nQuery is Modifies(assignment, variable) \n"; 
	printResults(pkb->getModifies(assignment, variable));

	cout << "\n\nQuery is Modifies(while, variable) \n"; 
	printResults(pkb->getModifies(whileLoop, variable));

	cout << "\n\nQuery is Modifies(statement, \"y\") \n"; 
	printResults(pkb->getModifies(statement, "y"));

	cout << "\n\nQuery is Modifies(\"test\", v) \n"; 
	printResults(pkb->getModifies("test", variable));

	cout << "\n\nQuery is Modifies(procedure, v) \n"; 
	printResults(pkb->getModifies(procedure, variable));

	cout << "\n\nQuery is Uses(\"test\", variable) \n"; 
	printResults(pkb->getUses("test", variable));

	cout << "\n\nQuery is Uses(\"5\", variable) \n"; 
	printResults(pkb->getUses("5", variable));

	cout << "\n\nQuery is Uses(assignment, variable) \n"; 
	printResults(pkb->getUses(assignment, variable));

	cout << "\n\nQuery is Uses(whileLoop, variable) \n"; 
	printResults(pkb->getUses(whileLoop, variable));

	cout << "\n\nQuery is Uses(statement, \"y\") \n"; 
	printResults(pkb->getUses(statement, "y"));

	cout << "\n\nQuery is Uses(statement, variable) \n"; 
	printResults(pkb->getUses(statement, variable));

	cout << "\n\nQuery is follows(\"2\", statement) \n"; 
	printResults(pkb->getFollows("2", statement));

	cout << "\n\nQuery is follows(whileLoop, statement) \n"; 
	printResults(pkb->getFollows(whileLoop, statement));

	cout << "\n\nQuery is follows(statement, \"6\") \n"; 
	printResults(pkb->getFollows(statement, "6"));

	cout << "\n\nQuery is Follows*(statement, assignment) \n"; 
	printResults(pkb->getFollowsStar(statement, assignment));

	cout << "\n\nQuery is Follows*(\"1\", assignment) \n"; 
	printResults(pkb->getFollowsStar("1", assignment));

	cout << "\n\nQuery is Follows*(assignment, \"4\") \n"; 
	printResults(pkb->getFollowsStar(assignment, "4"));

	cout << "\n\nQuery is Follows*(\"1\", \"3\") \n"; 
	printResults(pkb->getFollowsStar("1", "3"));

	cout << "\n\nQuery is follows(statement, assignment) \n"; 
	printResults(pkb->getFollows(statement, assignment));

	cout << "\n\nQuery is Parents(\"4\", statement) \n"; 
	printResults(pkb->getParents("4", statement));

	cout << "\n\nQuery is Parents(whileLoop, statement) \n"; 
	printResults(pkb->getParents(whileLoop, statement));

	cout << "\n\nQuery is Parents(statement, \"6\") \n"; 
	printResults(pkb->getParents(statement, "6"));

	cout << "\n\nQuery is Parents(statement, assignment) \n"; 
	printResults(pkb->getParents(statement, assignment));

	cout << "\n\nQuery is Parents*(statement, assignment) \n"; 
	printResults(pkb->getParentsStar(statement, assignment));

	cout << "\n\nQuery is Parents*(\"4\", assignment) \n"; 
	printResults(pkb->getParentsStar("4", assignment));

	cout << "\n\nQuery is Parents*(whileLoop, \"8\") \n"; 
	printResults(pkb->getParentsStar(whileLoop, "8"));

	cout << "\n\nQuery is Parents*(\"4\", \"8\") \n"; 
	printResults(pkb->getParentsStar("4", "8"));

	cout << "\n\nQuery is Calls(\"test\", procedure) \n"; 
	printResults(pkb->getCalls("test", procedure));

	cout << "\n\nQuery is Calls(procedure, procedure) \n"; 
	printResults(pkb->getCalls(procedure, procedure));

	cout << "\n\nQuery is Calls(procedure, \"test3\") \n"; 
	printResults(pkb->getCalls(procedure, "test3"));

	cout << "\n\nQuery is Calls(\"test\", \"test2\") \n"; 
	printResults(pkb->getCalls("test", "test2"));

	cout << "\n\nQuery is Calls*(\"test\", procedure) \n"; 
	printResults(pkb->getCallsStar("test", procedure));

	cout << "\n\nQuery is Calls*(procedure, procedure) \n"; 
	printResults(pkb->getCallsStar(procedure, procedure));

	cout << "\n\nQuery is Calls*(procedure, \"test3\") \n"; 
	printResults(pkb->getCallsStar(procedure, "test3"));

	cout << "\n\nQuery is Calls*(\"test\", \"test3\") \n"; 
	printResults(pkb->getCallsStar("test", "test3"));

	cout << "\n\nQuery is Pattern assignment(variable, _\"z + y\"_) \n"; 
	printResults(pkb->searchWithPattern(assignment, "_", "_z + y_"));

	cout << "\n\nQuery is Pattern assignment(variable, \"g + h\") \n"; 
	printResults(pkb->searchWithPattern(assignment, "_", "g + h"));

	cout << "\n\nQuery is Pattern whileLoop(variable, \"_\") \n"; 
	printResults(pkb->searchWithPattern(whileLoop, "_", "_"));

	cout << "\n\nQuery is Pattern assignment(variable, \"_x_\") \n"; 
	printResults(pkb->searchWithPattern(assignment, "_", "_x_"));

	cout << "\n\nQuery is Pattern assignment(\"f\", \"_\") \n"; 
	printResults(pkb->searchWithPattern(assignment, "f", "_"));

	cin.get();
}

void _tmain(int argc, _TCHAR* argv[]) {
	PKB_test test = PKB_test();
	test.runTests();
}