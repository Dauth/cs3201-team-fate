#include "stdafx.h"
#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator(PKB* p, DesignExtractor* de) {
	pkb = p;
	dEx = de;
}

void QueryEvaluator::evaluate(Symbol* s, QueryTree* qt) {
	hasResult = true;
	symbol = s;
	tree = qt;
	evaluatePattern();
	evaluateQuery();
	evaluateResult();
}

void QueryEvaluator::evaluatePattern() {
	PatternNode* pattern = tree->getPattern();
	std::vector<Node*> result;
	while(pattern) {
		if(pattern->getMiddleParam()->getType() == expression) {
			result = dEx->searchWithPattern(pattern->getLeftParam()->getType(), pattern->getMiddleParam()->getParam(), pattern->getRightParam()->getParam());
		} else {
			result = dEx->searchWithPattern(pattern->getLeftParam()->getType(), pattern->getMiddleParam()->getType(), pattern->getRightParam()->getParam());
		}
		hasResult = resultNotEmpty(pattern->getLeftParam(), result);
	}
}

void QueryEvaluator::evaluateQuery() {
	QueryNode* query = tree->getQuery();
	std::vector<Node*> result;
	while(query) {
		ParamNode* left = query->getLeftParam();
		ParamNode* right = query->getRightParam();
		std::vector<Node*> result;
		ParamNode* arg = getOptimal(left, right);
		if(left == arg) {
			result = evaluateLeft(query->getType(), arg);
			hasResult = resultNotEmpty(right, result);
		} else {
			result = evaluateRight(query->getType(), arg);
			hasResult = resultNotEmpty(left, result);
		}
		query = tree->getQuery();
	}
}

std::vector<Node*> QueryEvaluator::evaluateLeft(query_type type, ParamNode* pnode) {
	if(pnode->getType() == integer) {
		return evaluateLeftByType(type, std::stoi(pnode->getParam()));
	} else if(pnode->getType() == expression) {
		return evaluateLeftByType(type, pnode->getParam());
	} else {
		return evaluateLeftByType(type, pnode->getType());
	}
}

std::vector<Node*> QueryEvaluator::evaluateRight(query_type type, ParamNode* pnode) {
	if(pnode->getType() == integer) {
		return evaluateRightByType(type, std::stoi(pnode->getParam()));
	} else if(pnode->getType() == expression) {
		return evaluateRightByType(type, pnode->getParam());
	} else {
		return evaluateRightByType(type, pnode->getType());
	}
}

std::vector<Node*> QueryEvaluator::evaluateLeftByType(query_type type, int lineNum) {
	switch(type) {
		case follows		:
			return pkb->getFollowing(lineNum);
		case followsStar	:
			return ;
		case parent			:
			return pkb->getChildren(lineNum);
		case parentStar		:
			return ;
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
	}
}

std::vector<Node*> QueryEvaluator::evaluateLeftByType(query_type type, std::string expr) {
	switch(type) {
		case modifies		:
			return pkb->getModifiedBy(expr);
		case uses			:
			return pkb->getUsedBy(expr);
		case follows		:
			return pkb->getFollowing(statement);
		case followsStar	:
			return ;
		case parent			:
			return pkb->getChildren(statement);
		case parentStar		:
			return ;
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
	}
}

std::vector<Node*> QueryEvaluator::evaluateLeftByType(query_type qType, synt_type sType) {
	switch(qType) {
		case modifies		:
			return pkb->getModifiedBy(sType);
		case uses			:
			return pkb->getUsedBy(sType);
		case follows		:
			return pkb->getFollowing(sType);
		case followsStar	:
			return ;
		case parent			:
			return pkb->getChildren(sType);
		case parentStar		:
			return ;
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
	}
}

std::vector<Node*> QueryEvaluator::evaluateRightByType(query_type type, int lineNum) {
	switch(type) {
		case modifies		:
			return pkb->getModifies(lineNum);
		case uses			:
			return pkb->getUses(lineNum);
		case follows		:
			return pkb->getFollowedBy(lineNum);
		case followsStar	:
			return ;
		case parent			:
			return pkb->getParent(lineNum);
		case parentStar		:
			return ;
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
	}
}

std::vector<Node*> QueryEvaluator::evaluateRightByType(query_type type, std::string expr) {
	switch(type) {
		case modifies		:
			return pkb->getModifies(expr);
		case uses			:
			return pkb->getUses(expr);
		case follows		:
			return pkb->getFollowedBy(statement);
		case followsStar	:
			return ;
		case parent			:
			return pkb->getParents(statement);
		case parentStar		:
			return ;
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
	}
}

std::vector<Node*> QueryEvaluator::evaluateRightByType(query_type qType, synt_type sType) {
	switch(qType) {
		case modifies		:
			return pkb->getModifies(sType);
		case uses			:
			return pkb->getUses(sType);
		case follows		:
			return pkb->getFollowedBy(sType);
		case followsStar	:
			return ;
		case parent			:
			return pkb->getParents(sType);
		case parentStar		:
			return ;
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
	}
}

ParamNode* getOptimal(ParamNode* left, ParamNode* right) {
	int leftTypeNum = std::numeric_limits<int>::max();
	int rightTypeNum = std::numeric_limits<int>::max();
	if(left->getParam() != "_") {
		leftTypeNum = pkb->getCount(left->getType());
	}
	if(right->getParam() != "_") {
		rightTypeNum = pkb->getCount(right->getType());
	}
	if(left->getType() == integer) {
		return left;
	} else if(right->getType() == integer) {
		return right;
	} else if(left->getType() == expression && left->getParam() == "_") {
		return left;
	} else if(right->getType() == expression && right->getParam() == "_") {
		return right;
	} else if(leftTypeNum < rightTypeNum) {
		return left;
	} else {
		return right;
	}
}

bool resultNotEmpty(ParamNode* pNode, std::vector<Node*> nVec) {
	int symIndex = -1;
	for(int i = 0; i < symbol->getSize(); i++) {
		if(symbol->getData(i).getVar() == pNode->getParam()) {
			symIndex = i;
		}
	}
	std::vector<Node*> resultVec;
	for(int i = 0; i < nVec.size(); i++) {
		if(pNode->getType() == nVec.at(i)->getType()) {
			resultVec.push_back(nVec.at(i));
		}
	}
	std::vector<Node*> symVec = symbol->getData(symIndex).getPKBOutput();
	if(resultVec.size() == 0) {
		return false;
	} else if(symVec.size() == 0) {
		symbol->getData(symIndex).setPKBOutput(resultVec);
	} else {
		std::vector<Node*> newVec;
		for(int i = 0; i < resultVec.size(); i++) {
			for(int j = 0; j < symVec.size(); j++) {
				if(resultVec.at(i) == symVec.at(j)) {
					newVec.push_back(resultVec.at(i));
					break;
				}
			}
		}
		symbol->getData(symIndex).setPKBOutput(newVec);
	}
	return true;
}

void QueryEvaluator::evaluateResult() {
	for(int i = 0; i < symbol->getSize(); i++) {
		if(symbol->getData(i).