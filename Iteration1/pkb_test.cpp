// spa_build.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "PKB.h"


void setupTestData(PKB &pkb) {
	// procedure test {
	Node* proc = pkb.createProcedure("test");
	Node* testStmtLst1 = pkb.createNode(statementList, 1);
	proc->setLeftChild(testStmtLst1);

	// 1. y = 3
	Node* s1 = pkb.createNode(assignment, 1, "", nullptr, nullptr, nullptr, proc);
	Node* s1a = pkb.createNode(variable, 1, "y", nullptr, s1, nullptr, proc);
	Node* s1c3 = pkb.createNode(constant, 1, "3");
	s1->setLeftChild(s1a);
	s1->setRightChild(s1c3);
	testStmtLst1->addStmt(s1);

	// 2. x = 4
	Node* s2 = pkb.createNode(assignment, 2, "", nullptr, nullptr, nullptr, proc);
	Node* s2a = pkb.createNode(variable, 2, "x", nullptr, s2, nullptr, proc);
	Node* s2c4 = pkb.createNode(constant, 2, "4");
	s2->setLeftChild(s2a);
	s2->setRightChild(s2c4);
	testStmtLst1->addStmt(s2);

	// 3. z = x + y
	Node* s3 = pkb.createNode(assignment, 3, "", nullptr, nullptr, nullptr, proc);
	Node* s3a = pkb.createNode(variable, 3, "z", nullptr, s3, nullptr, proc);
	Node* s3v_x = pkb.createNode(variable, 3, "x", s3, nullptr, nullptr, proc);
	Node* s3e_plus = pkb.createNode(expression, 3, "+", s3, nullptr, nullptr, proc);
	Node* s3v_y = pkb.createNode(variable, 3, "y", s3, nullptr, nullptr, proc);
	s3->setLeftChild(s3a);
	s3->setRightChild(s3e_plus);
	s3e_plus->setLeftChild(s3v_x);
	s3e_plus->setRightChild(s3v_y);
	testStmtLst1->addStmt(s3);

	// 4. While x {
	Node* s4 = pkb.createNode(whileLoop, 4, "", nullptr, nullptr, nullptr, proc);
	Node* s4v_x = pkb.createNode(variable, 4, "x", s4, nullptr, nullptr, proc);
	Node* whileStmtLst = pkb.createNode(statementList, 4);
	s4->setLeftChild(s4v_x);
	s4->setRightChild(whileStmtLst);
	testStmtLst1->addStmt(s4);

	// 5. a = z + y + x
	Node* s5 = pkb.createNode(assignment, 5, "", nullptr, nullptr, s4, proc);
	Node* s5a = pkb.createNode(variable, 5, "a", nullptr, s5, s4, proc);
	Node* s5v_z = pkb.createNode(variable, 5, "z", s5, nullptr, s4, proc);
	Node* s5e_plus1 = pkb.createNode(expression, 5, "+", s5, nullptr, nullptr, proc);
	Node* s5v_y = pkb.createNode(variable, 5, "y", s5, nullptr, s4, proc);
	Node* s5e_plus2 = pkb.createNode(expression, 5, "+", s5, nullptr, nullptr, proc);
	Node* s5v_x = pkb.createNode(variable, 5, "x", s5, nullptr, s4, proc);
	s5->setLeftChild(s5a);
	s5->setRightChild(s5e_plus1);
	s5e_plus1->setLeftChild(s5e_plus2);
	s5e_plus1->setRightChild(s5v_x);
	s5e_plus2->setLeftChild(s5v_z);
	s5e_plus2->setRightChild(s5v_y);
	whileStmtLst->addStmt(s5);

	// 6. b = a + z
	Node* s6 = pkb.createNode(assignment, 6, "", nullptr, nullptr, s4, proc);
	Node* s6a = pkb.createNode(variable, 6, "b", nullptr, s6, s4, proc);
	Node* s6v_a = pkb.createNode(variable, 6, "a", s6, nullptr, s4, proc);
	Node* s6e_plus = pkb.createNode(expression, 6, "+", s6, nullptr, nullptr, proc);
	Node* s6v_z = pkb.createNode(variable, 6, "z", s6, nullptr, s4, proc);
	s6->setLeftChild(s6a);
	s6->setRightChild(s6e_plus);
	s6e_plus->setLeftChild(s6v_a);
	s6e_plus->setRightChild(s6v_z);
	whileStmtLst->addStmt(s6);

	// 7. While y {
	Node* s7 = pkb.createNode(whileLoop, 7, "", nullptr, nullptr, s4, proc);
	Node* s7v_x = pkb.createNode(variable, 7, "y", s7, nullptr, s4, proc);
	Node* whileStmtLst2 = pkb.createNode(statementList, 7);
	s7->setLeftChild(s7v_x);
	s7->setRightChild(whileStmtLst2);
	whileStmtLst->addStmt(s7);


	// 8. t = a + x }
	Node* s8 = pkb.createNode(assignment, 8, "", nullptr, nullptr, s7, proc);
	Node* s8a = pkb.createNode(variable, 8, "t", nullptr, s8, s7, proc);
	Node* s8v_a = pkb.createNode(variable, 8, "a", s8, nullptr, s7, proc);
	Node* s8e_plus = pkb.createNode(expression, 8, "+", s8, nullptr, nullptr, proc);
	Node* s8v_z = pkb.createNode(variable, 8, "x", s6, nullptr, s7, proc);
	s8->setLeftChild(s8a);
	s8->setRightChild(s8e_plus);
	s8e_plus->setLeftChild(s8v_a);
	s8e_plus->setRightChild(s8v_z);
	whileStmtLst2->addStmt(s8);

	// 9. c = z + x }
	Node* s9 = pkb.createNode(assignment, 9, "", nullptr, nullptr, s4, proc);
	Node* s9a = pkb.createNode(variable, 9, "c", nullptr, s9, s4, proc);
	Node* s9v_z = pkb.createNode(variable, 9, "z", s9, nullptr, s4, proc);
	Node* s9e_plus = pkb.createNode(expression, 9, "+", s9, nullptr, nullptr, proc);
	Node* s9v_x = pkb.createNode(variable, 9, "x", s9, nullptr, s4, proc);
	s9->setLeftChild(s9a);
	s9->setRightChild(s9e_plus);
	s9e_plus->setLeftChild(s9v_z);
	s9e_plus->setRightChild(s9v_x);
	whileStmtLst->addStmt(s9);

	// 10. x = x + z
	Node* s10 = pkb.createNode(assignment, 10, "", nullptr, nullptr, nullptr, proc);
	Node* s10a = pkb.createNode(variable, 10, "x", nullptr, s10, nullptr, proc);
	Node* s10v_x = pkb.createNode(variable, 10, "x", s10, nullptr, nullptr, proc);
	Node* s10e_plus = pkb.createNode(expression, 10, "+", s10, nullptr, nullptr, proc);
	Node* s10v_z = pkb.createNode(variable, 10, "z", s10, nullptr, nullptr, proc);
	s10->setLeftChild(s10a);
	s10->setRightChild(s10e_plus);
	s10e_plus->setLeftChild(s10v_x);
	s10e_plus->setRightChild(s10v_z);
	testStmtLst1->addStmt(s10);

	// 11. a = 3 + y
	Node* s11 = pkb.createNode(assignment, 11, "", nullptr, nullptr, nullptr, proc);
	Node* s11a = pkb.createNode(variable, 11, "a", nullptr, s11, nullptr, proc);
	Node* s11c_3 = pkb.createNode(constant, 11, "3");
	Node* s11e_plus = pkb.createNode(expression, 11, "+", s11, nullptr, nullptr, proc);
	Node* s11v_z = pkb.createNode(variable, 11, "y", s11, nullptr, nullptr, proc);
	s11->setLeftChild(s11a);
	s11->setRightChild(s11e_plus);
	s11e_plus->setLeftChild(s11c_3);
	s11e_plus->setRightChild(s11v_z);
	testStmtLst1->addStmt(s11);
}

// Modifies ("test", v)
void testModifiesProc(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "Query is Modifies(\"test\", v) \n"; 
	result = pkb.getModifies("test");
	std::cout << "Obtained " << result.size() << " results\n"; // 9
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", " << result[i]->getVariable()->getName() << " ) ";
	} 
}

// Modifies (5, v)
void testModifiesStmt(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\n\Query is Modifies(5, v) \n"; 
	result = pkb.getModifies(5);
	std::cout << "Obtained " << result.size() << " results\n"; // 1
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", " << result[i]->getVariable()->getName() << " ) ";
	} 
}

	// Modifies (assignment, v)
void testModifiesAssign(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\n\Query is Modifies(assignment, v) \n"; 
	result = pkb.getModifies(assignment);
	std::cout << "Obtained " << result.size() << " results\n"; // 9
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", " << result[i]->getVariable()->getName() << " ) ";
	} 
}

	// Modifies (whileLoop, v)
void testModifiesWhile(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\n\Query is Modifies(whileLoop, v) \n"; 
	result = pkb.getModifies(whileLoop);
	std::cout << "Obtained " << result.size() << " results\n"; // 4
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", " << result[i]->getVariable()->getName() << " ) ";
	} 
}

	// Modifies (s/p, "y")
void testModifiedBySpecifiedVar(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\n\Query is Modifies(s/p, \"y\") \n"; 
	result = pkb.getModifiedBy("y");
	std::cout << "Obtained " << result.size() << " results\n"; // 2
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", \"y\" ) ";
	} 
}

	// Modifies (s/p, variable)
void testModifiedByVarType(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\nQeury is Modifies(s/p, variable) \n"; 
	result = pkb.getModifiedBy(variable);
	std::cout << "Obtained " << result.size() << " results\n"; // 12
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", v ) ";
	}

}

	// Modifies (s/p, "t")
void testModifiedByNested(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\n\Query is Modifies(s/p, \"t\") \n"; 
	result = pkb.getModifiedBy("t");
	std::cout << "Obtained " << result.size() << " results\n"; // 4
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", \"y\" ) ";
	} 
}

	// Uses ("test", v)
void testUsesProc(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\nQuery is Uses(\"test\", v) \n"; 
	result = pkb.getUses("test");
	std::cout << "Obtained " << result.size() << " results\n"; // 16
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", " << result[i]->getVariable()->getName() << " ) ";
	} 
}

	// Uses (5, v)
void testUsesStmt(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\n\Query is Uses(5, v) \n"; 
	result = pkb.getUses(5);
	std::cout << "Obtained " << result.size() << " results\n"; // 3
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", " << result[i]->getVariable()->getName() << " ) ";
	}
}

	// Uses (assignment, v)
void testUsesAssign(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\n\Query is Uses(assignment, v) \n"; 
	result = pkb.getUses(assignment);
	std::cout << "Obtained " << result.size() << " results\n"; // 14
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", " << result[i]->getVariable()->getName() << " ) ";
	} 
}

	// Uses (whileLoop, v)
void testUsesWhile(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\n\Query is Uses(whileLoop, v) \n"; 
	result = pkb.getUses(whileLoop);
	std::cout << "Obtained " << result.size() << " results\n"; // 11
	for(int i=0; i<result.size(); i++) {
		std::cout << "( whileLoop, " << result[i]->getVariable()->getName() << " ) ";
	} 
}

	// Uses (s, "y")
void testUsedBySpecificVar(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\n\Query is Uses(s/p, \"y\") \n"; 
	result = pkb.getUsedBy("y");
	std::cout << "Obtained " << result.size() << " results\n"; // 6
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", \"y\" ) ";
	} 
}

	// Uses (s, variable)
void testUsedByVarType(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\n\Query is Uses(s/p, variable) \n"; 
	result = pkb.getUsedBy(variable);
	std::cout << "Obtained " << result.size() << " results\n"; // 10
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", v ) ";
	} 
}

	// Parent (4, s)
void testParentStatement(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\nQuery is Parent(4, s) \n"; 
	result = pkb.getChildren(4);
	std::cout << "Obtained " << result.size() << " results\n"; // 4
	for(int i=0; i<result.size(); i++) {
		std::cout << "( 4, " << result[i]->getLine() << " ) ";
	} 
}

	// Parent (whileLoop, s)
void testParentWhile(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\nQuery is Parent(whileLoop, s) \n"; 
	result = pkb.getChildren(whileLoop);
	std::cout << "Obtained " << result.size() << " results\n"; // 5
	for(int i=0; i<result.size(); i++) {
		std::cout << "( whileLoop, " << result[i]->getLine() << " ) ";
	} 
}

	// Parent (s, 6)
void testChildStatement(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\nQuery is Parent(s, 6) \n"; 
	result = pkb.getParent(6);
	std::cout << "Obtained " << result.size() << " results\n"; // 1
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", 6 ) ";
	} 
}

	// Parent (s, assignment)
void testChildAssign(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\nQuery is Parent(s, assignment) \n"; 
	result = pkb.getParents(assignment);
	std::cout << "Obtained " << result.size() << " results\n"; // 2
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", a ) ";
	} 
}

	// follows (2, s)
void testFollowsStatement(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\nQuery is Follows(2, s) \n"; 
	result = pkb.getFollowing(2);
	std::cout << "Obtained " << result.size() << " results\n"; // 1
	for(int i=0; i<result.size(); i++) {
		std::cout << "( 2, " << result[i]->getLine() << " ) ";
	} 
}

	// follows (whileLoop, s)
void testFollowsWhile(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\nQuery is Follows(whileLoop, s) \n"; 
	result = pkb.getFollowing(whileLoop);
	std::cout << "Obtained " << result.size() << " results\n"; // 2
	for(int i=0; i<result.size(); i++) {
		std::cout << "( whileLoop, " << result[i]->getLine() << " ) ";
	}
}

	// follows (s, 6)
void testFollowedByStatement(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\nQuery is follows(s, 6) \n"; 
	result = pkb.getFollowedBy(6);
	std::cout << "Obtained " << result.size() << " results\n"; // 1
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", 6 ) ";
	} 
}

	// follows (s, assignment)
void testFollowedByAssignment(PKB &pkb) {
	std::vector<Node*> result;
	std::cout << "\n\nQuery is follows(s, assignment) \n"; 
	result = pkb.getFollowedBy(assignment);
	std::cout << "Obtained " << result.size() << " results\n"; // 6
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", a ) ";
	} 
}

int _tmain(int argc, _TCHAR* argv[]) {

	PKB pkb;

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

	setupTestData(pkb);
	// Modifies ("test", v)
	testModifiesProc(pkb);
	// Modifies (5, v)
	testModifiesStmt(pkb);
	// Modifies (assignment, v)
	testModifiesAssign(pkb);
	// Modifies (whileLoop, v)
	testModifiesWhile(pkb);
	// Modifies (s/p, "y")
	testModifiedBySpecifiedVar(pkb);
	// Modifies (s/p, variable)
	testModifiedByVarType(pkb);
	// Modifies (s/p, "t")
	testModifiedByNested(pkb);
	// Uses ("test", v)
	testUsesProc(pkb);
	// Uses (5, v)
	testUsesStmt(pkb);
	// Uses (assignment, v)
	testUsesAssign(pkb);
	// Uses (whileLoop, v)
	testUsesWhile(pkb);
	// Uses (s, "y")
	testUsedBySpecificVar(pkb);
	// Uses (s, variable)
	testUsedByVarType(pkb);
	// Parent (4, s)
	testParentStatement(pkb);
	// Parent (whileLoop, s)
	testParentWhile(pkb);
	// Parent (s, 6)
	testChildStatement(pkb);
	// Parent (s, assignment)
	testChildAssign(pkb);
	// follows (2, s)
	testFollowsStatement(pkb);
	// follows (whileLoop, s)
	testFollowsWhile(pkb);
	// follows (s, 6)
	testFollowedByStatement(pkb);
	// follows (s, assignment)
	testFollowedByAssignment(pkb);

	std::cin.get();
}