// spa_build.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "QueryEvaluator.h"
#include "AST.h"
#include "SimpleParser.h"

int _tmain(int argc, _TCHAR* argv[]) {
	PKB pkb;
	ExpressionTree et = ExpressionTree(&pkb);
	AST ast = AST(&pkb, &et);
	SimpleParser sp = SimpleParser(&ast);
	DesignExtractor de = DesignExtractor(&pkb, &et);
	QueryEvaluator qe = QueryEvaluator(&pkb, &de);

	std::vector<std::string> r = sp.readSourceFile("C:\\Users\\temporal\\Documents\\simpletest.txt");
	std::vector<Node*> a = sp.execute(r);

	std::cout<< "variable v; assign a; stmt s; while w;";
	Symbol* sym = new Symbol("v", variable);
	sym->setVar("a", assignment);
	sym->setVar("s", statement);
	sym->setVar("w", whileLoop);

	std::cout<< "Select v such that\n";
	int index = sym->getIndex("v");
	//sym->setResult(index);

	// Modifies ("test", v)
	std::cout << "Query is pattern w(\"y\",_) such that Modifies(w,v)\n";
	QueryTree* tree = new QueryTree();
	tree->addPattern(new PatternNode(new ParamNode(whileLoop, "w"), new ParamNode(expression, "y"), new ParamNode(expression, "_")));
	tree->addQuery(new QueryNode(modifies, new ParamNode(whileLoop, "w"), new ParamNode(variable, "v")));
	qe.evaluate(sym, tree);

	sym = new Symbol("v", variable);
	sym->setVar("a", assignment);
	sym->setVar("s", statement);
	sym->setVar("w", whileLoop);

	index = sym->getIndex("v");
	sym->setResult(index);

	// Modifies (5, v)
	std::cout << "\n\n\Query is Modifies(5, v) \n"; 
	tree = new QueryTree();
	tree->addQuery(new QueryNode(modifies, new ParamNode(integer, "5"), new ParamNode(variable, "v")));
	qe.evaluate(sym, tree);
	
	sym = new Symbol("v", variable);
	sym->setVar("a", assignment);
	sym->setVar("s", statement);
	sym->setVar("w", whileLoop);

	index = sym->getIndex("v");
	sym->setResult(index);

	// Modifies ("test", v)
	std::cout << "\n\nQeury is Modifies(\"test\", v) and Query is Modifies(1, v) \n";
	tree = new QueryTree();
	tree->addQuery(new QueryNode(modifies, new ParamNode(expression, "test"), new ParamNode(variable, "v")));
	tree->addQuery(new QueryNode(modifies, new ParamNode(integer, "1"), new ParamNode(variable, "v")));
	qe.evaluate(sym, tree);
	
	sym = new Symbol("v", variable);
	sym->setVar("a", assignment);
	sym->setVar("s", statement);
	sym->setVar("w", whileLoop);

	index = sym->getIndex("a");
	sym->setResult(index);

	// Modifies (assignment, v)
	std::cout << "\n\n\Query is Modifies(\"test\",v) and Modifies(a,v) \n"; 
	tree = new QueryTree();
	tree->addQuery(new QueryNode(modifies, new ParamNode(expression, "test"), new ParamNode(variable, "v")));
	tree->addQuery(new QueryNode(modifies, new ParamNode(assignment, "a"), new ParamNode(variable, "v")));
	qe.evaluate(sym, tree);

	// Modifies (s/p, "y")
	std::cout << "\n\n\Query is Modifies(s/p, \"t\") \n"; 
	sym = new Symbol("v", variable);
	sym->setVar("a", assignment);
	sym->setVar("s", statement);
	sym->setVar("w", whileLoop);

	index = sym->getIndex("s");
	sym->setResult(index);

	tree = new QueryTree();
	tree->addQuery(new QueryNode(modifies, new ParamNode(statement, "s"), new ParamNode(expression, "t")));
	qe.evaluate(sym, tree);

	// Modifies (s/p, "y")
	sym = new Symbol("v", variable);
	sym->setVar("a", assignment);
	sym->setVar("s", statement);
	sym->setVar("w", whileLoop);

	index = sym->getIndex("s");
	sym->setResult(index);

	// Modifies ("test", v)
	std::cout << "\n\nQeury is Modifies(s/p, variable) \n";  
	tree = new QueryTree();
	tree->addQuery(new QueryNode(modifies, new ParamNode(statement, "s"), new ParamNode(variable, "v")));
	qe.evaluate(sym, tree);

	// Uses ("test", v)
	sym = new Symbol("v", variable);
	sym->setVar("a", assignment);
	sym->setVar("s", statement);
	sym->setVar("w", whileLoop);

	index = sym->getIndex("v");
	sym->setResult(index);

	// Modifies ("test", v)
	std::cout << "\n\nQuery is Uses(\"test\", v) \n";  
	tree = new QueryTree();
	tree->addQuery(new QueryNode(uses, new ParamNode(expression, "test"), new ParamNode(variable, "v")));
	qe.evaluate(sym, tree);

	// Uses (5, v)
	sym = new Symbol("v", variable);
	sym->setVar("a", assignment);
	sym->setVar("s", statement);
	sym->setVar("w", whileLoop);

	index = sym->getIndex("v");
	sym->setResult(index);

	// Modifies ("test", v)
	std::cout << "\n\n\Query is Uses(5, v) \n"; 
	tree = new QueryTree();
	tree->addQuery(new QueryNode(uses, new ParamNode(integer, "5"), new ParamNode(variable, "v")));
	qe.evaluate(sym, tree);

	// Uses (assignment, v)
	sym = new Symbol("v", variable);
	sym->setVar("a", assignment);
	sym->setVar("s", statement);
	sym->setVar("w", whileLoop);

	index = sym->getIndex("v");
	sym->setResult(index);

	// Modifies ("test", v)
	std::cout << "\n\n\Query is Uses(assignment, v) \n";
	tree = new QueryTree();
	tree->addQuery(new QueryNode(uses, new ParamNode(assignment, "a"), new ParamNode(variable, "v")));
	qe.evaluate(sym, tree);

	// Uses (s, "y")
	sym = new Symbol("v", variable);
	sym->setVar("a", assignment);
	sym->setVar("s", statement);
	sym->setVar("w", whileLoop);

	index = sym->getIndex("s");
	sym->setResult(index);

	// Modifies ("test", v)
	std::cout << "\n\n\Query is Uses(s/p, \"y\") \n";
	tree = new QueryTree();
	tree->addQuery(new QueryNode(uses, new ParamNode(statement, "s"), new ParamNode(expression, "y")));
	qe.evaluate(sym, tree);

	/* Uses (s, variable)
	std::cout << "\n\n\Query is Uses(s/p, variable) \n"; 
	result = pkb.getUsedBy(variable);
	std::cout << "Obtained " << result.size() << " results\n"; // 8
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", v ) ";
	} 

	// Parent (4, s)
	std::cout << "\n\nQuery is Parent(4, s) \n"; 
	result = pkb.getChildren(4);
	std::cout << "Obtained " << result.size() << " results\n"; // 3
	for(int i=0; i<result.size(); i++) {
		std::cout << "( 4, " << result[i]->getLine() << " ) ";
	} 

	// Parent (whileLoop, s)
	std::cout << "\n\nQuery is Parent(whileLoop, s) \n"; 
	result = pkb.getChildren(whileLoop);
	std::cout << "Obtained " << result.size() << " results\n"; // 3
	for(int i=0; i<result.size(); i++) {
		std::cout << "( 4, " << result[i]->getLine() << " ) ";
	} 

	// Parent (s, 6)
	std::cout << "\n\nQuery is Parent(s, 6) \n"; 
	result = pkb.getParent(6);
	std::cout << "Obtained " << result.size() << " results\n"; // 1
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", 6 ) ";
	} 

	// Parent (s, assignment)
	std::cout << "\n\nQuery is Parent(s, assignment) \n"; 
	result = pkb.getParents(assignment);
	std::cout << "Obtained " << result.size() << " results\n"; // 1
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", a ) ";
	} 

	// follows (2, s)
	std::cout << "\n\nQuery is Follows(4, s) \n"; 
	result = pkb.getFollowing(2);
	std::cout << "Obtained " << result.size() << " results\n"; // 1
	for(int i=0; i<result.size(); i++) {
		std::cout << "( 2, " << result[i]->getLine() << " ) ";
	} 

	// follows (whileLoop, s)
	std::cout << "\n\nQuery is Follows(whileLoop, s) \n"; 
	result = pkb.getFollowing(whileLoop);
	std::cout << "Obtained " << result.size() << " results\n"; // 1
	for(int i=0; i<result.size(); i++) {
		std::cout << "( whileLoop, " << result[i]->getLine() << " ) ";
	} 

	// follows (s, 6)
	std::cout << "\n\nQuery is follows(s, 6) \n"; 
	result = pkb.getFollowedBy(6);
	std::cout << "Obtained " << result.size() << " results\n"; // 1
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", 6 ) ";
	} 

	// follows (s, assignment)
	std::cout << "\n\nQuery is Parent(s, assignment) \n"; 
	result = pkb.getFollowedBy(assignment);
	std::cout << "Obtained " << result.size() << " results\n"; // 1
	for(int i=0; i<result.size(); i++) {
		std::cout << "( " << result[i]->getLine() << ", a ) ";
	} */
	std::string s;
	bool on = true;
	while(on){
		std::cin>>s;
		if(s == "off"){
			on = false;
		}
	}
}

