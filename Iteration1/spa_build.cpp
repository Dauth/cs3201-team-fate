// spa_build.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "PKB.h"

int _tmain(int argc, _TCHAR* argv[])
{
  PKB pkb;

  // usedby, modifiedby, parent, procedure

  // procedure test {
  Node* proc = pkb.createProcedure("test");
  Node* testStmtLst1 = pkb.createNode(statementList, 1);
  proc->setLeftChild(testStmtLst1);

  // 1. y = 3
  Node* s1 = pkb.createNode(assignment, 1);
  Node* s1a = pkb.createNode(variable, 1, "y", nullptr, s1, nullptr, proc);
  Node* s1c3 = pkb.createNode(constant, 1, "3");
  s1->setLeftChild(s1a);
  s1->setRightChild(s1c3);
  testStmtLst1->addStmt(s1);

  // 2. x = 4
  Node* s2 = pkb.createNode(assignment, 2);
  Node* s2a = pkb.createNode(variable, 2, "x", nullptr, s2, nullptr, proc);
  Node* s2c4 = pkb.createNode(constant, 2, "4");
  s2->setLeftChild(s2a);
  s2->setRightChild(s2c4);
  testStmtLst1->addStmt(s2);

  // 3. z = x + y
  Node* s3 = pkb.createNode(assignment, 3);
  Node* s3a = pkb.createNode(variable, 3, "z", nullptr, s3, nullptr, proc);
  Node* s3v_x = pkb.createNode(variable, 3, "x", s3a, nullptr, nullptr, proc);
  Node* s3e_plus = pkb.createNode(expression, 3, "+", s3a, nullptr, nullptr, proc);
  Node* s3v_y = pkb.createNode(variable, 3, "y", s3a, nullptr, nullptr, proc);
  s3->setLeftChild(s3a);
  s3->setRightChild(s3e_plus);
  s3e_plus->setLeftChild(s3v_x);
  s3e_plus->setRightChild(s3v_y);
  testStmtLst1->addStmt(s3);

  // 4. While x {
  Node* s4w = pkb.createNode(whileLoop, 4);
  Node* s4v_x = pkb.createNode(variable, 4, "x", s4w, nullptr, nullptr, proc);
  Node* whileStmtLst = pkb.createNode(statementList, 4);
  s4w->setLeftChild(s4v_x);
  s4w->setRightChild(whileStmtLst);
  testStmtLst1->addStmt(s4w);

  // 5. a = z + y + x
  Node* s5 = pkb.createNode(assignment, 5);
  Node* s5a = pkb.createNode(variable, 5, "a", nullptr, s5, nullptr, proc);
  Node* s5v_z = pkb.createNode(variable, 5, "z", s5a, nullptr, nullptr, proc);
  Node* s5e_plus1 = pkb.createNode(expression, 5, "+", s5a, nullptr, nullptr, proc);
  Node* s5v_y = pkb.createNode(variable, 5, "y", s5a, nullptr, nullptr, proc);
  Node* s5e_plus2 = pkb.createNode(expression, 5, "+", s5a, nullptr, nullptr, proc);
  Node* s5v_x = pkb.createNode(variable, 5, "x", s5a, nullptr, nullptr, proc);
  s5->setLeftChild(s5a);
  s5->setRightChild(s5e_plus1);
  s5e_plus1->setLeftChild(s5e_plus2);
  s5e_plus1->setRightChild(s5v_x);
  s5e_plus2->setLeftChild(s5v_z);
  s5e_plus2->setRightChild(s5v_y);
  whileStmtLst->addStmt(s5);

  // 6. b = a + z
  Node* s6 = pkb.createNode(assignment, 6);
  Node* s6a = pkb.createNode(variable, 6, "b", nullptr, s6, nullptr, proc);
  Node* s6v_a = pkb.createNode(variable, 6, "a", s6a, nullptr, nullptr, proc);
  Node* s6e_plus = pkb.createNode(expression, 6, "+", s6a, nullptr, nullptr, proc);
  Node* s6v_z = pkb.createNode(variable, 6, "z", s6a, nullptr, nullptr, proc);
  s6->setLeftChild(s6a);
  s6->setRightChild(s6e_plus);
  s6e_plus->setLeftChild(s6v_a);
  s6e_plus->setRightChild(s6v_z);
  whileStmtLst->addStmt(s6);

  // 7. c = z + x }
  Node* s7 = pkb.createNode(assignment, 7);
  Node* s7a = pkb.createNode(variable, 7, "c", nullptr, s7, nullptr, proc);
  Node* s7v_z = pkb.createNode(variable, 7, "z", s7a, nullptr, nullptr, proc);
  Node* s7e_plus = pkb.createNode(expression, 7, "+", s7a, nullptr, nullptr, proc);
  Node* s7v_x = pkb.createNode(variable, 7, "x", s7a, nullptr, nullptr, proc);
  s7->setLeftChild(s7a);
  s7->setRightChild(s7e_plus);
  s7e_plus->setLeftChild(s7v_z);
  s7e_plus->setRightChild(s7v_x);
  whileStmtLst->addStmt(s7);

  // 8. x = x + z
  Node* s8 = pkb.createNode(assignment, 8);
  Node* s8a = pkb.createNode(variable, 8, "x", nullptr, s8, nullptr, proc);
  Node* s8v_x = pkb.createNode(variable, 8, "x", s8a, nullptr, nullptr, proc);
  Node* s8e_plus = pkb.createNode(expression, 8, "+", s8a, nullptr, nullptr, proc);
  Node* s8v_z = pkb.createNode(variable, 8, "z", s8a, nullptr, nullptr, proc);
  s8->setLeftChild(s8a);
  s8->setRightChild(s8e_plus);
  s8e_plus->setLeftChild(s8v_x);
  s8e_plus->setRightChild(s8v_z);
  testStmtLst1->addStmt(s8);

  // 9. a = 3 + y
  Node* s9 = pkb.createNode(assignment, 9);
  Node* s9a = pkb.createNode(variable, 9, "x", nullptr, s9, nullptr, proc);
  Node* s9c_3 = pkb.createNode(constant, 9, "3");
  Node* s9e_plus = pkb.createNode(expression, 9, "+", s9a, nullptr, nullptr, proc);
  Node* s9v_z = pkb.createNode(variable, 9, "y", s9a, nullptr, nullptr, proc);
  s9->setLeftChild(s9a);
  s9->setRightChild(s9e_plus);
  s9e_plus->setLeftChild(s9c_3);
  s9e_plus->setRightChild(s9v_z);
  testStmtLst1->addStmt(s9);

  // buggy quering
  std::vector<Node*> result = pkb.getUses(assignment);
  std::cout << result.size() << "\n";

  for(std::vector<Node*>::iterator it = result.begin(); it != result.end(); ++it) {
	  std::cout << "statement no : " << (*it)->getLine() << "\n";
  }
  std::cin.get();
}

