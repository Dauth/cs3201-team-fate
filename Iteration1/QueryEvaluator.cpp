#include "stdafx.h"
#include "QueryEvaluator.h"
#include "PKB.h"

QueryEvaluator::QueryEvaluator() {
	hasResult = true;
}

void QueryEvaluator::evaluate(SymbolTable* st, QueryTree* qt) {
	table = st;
	tree = qt;
	evaluatePattern();
}

void QueryEvaluator::evaluatePattern() {
	//tree->getPattern()...
}

void QueryEvaluator::evaluateQuery() {
	QueryNode* query = tree->getQuery();
	while(query) {
		ParamNode* left = query->getLeftParam();
		ParamNode* right = query->getRightParam();
		std::vector<Node*> result;
		ParamNode* arg = getOptimal(left, right);
		if(left == arg) {
			evaluateLeft(query->getType(), arg);
		} else {
			evaluateRight(query->getType(), arg);
		}
		query = tree->getQuery();
	}
}

void QueryEvaluator::evaluateLeft(query_type type, ParamNode* pnode) {
	if(pnode->getType() == integer) {
		evaluateLeftByType(type, std::stoi(pnode->getParam()));
	} else if(pnode->getType() == expression) {
		evaluateLeftByType(type, pnode->getParam());
	} else {
		evaluateLeftByType(type, pnode->getType());
	}
}

void QueryEvaluator::evaluateLeftByType(query_type type, int lineNum) {
	switch(type) {
		case modifies		:

		case uses			:
		case follows		:
		case followsStar	:
		case parent			:
		case parentStar		:
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
	}
}