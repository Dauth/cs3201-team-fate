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
			result = dEx->searchWithPattern(pattern->getLeftParam()->getType(), "_", pattern->getRightParam()->getParam());
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
			return dEx->getFollowingStar(lineNum);
		case parent			:
			return pkb->getChildren(lineNum);
		case parentStar		:
			return dEx->getChildrenStar(lineNum);
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
		default				:
			std::vector<Node*> empty;
			return empty;
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
			return dEx->getFollowingStar(statement);
		case parent			:
			return pkb->getChildren(statement);
		case parentStar		:
			return dEx->getChildrenStar(statement);
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
		default				:
			std::vector<Node*> empty;
			return empty;
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
			return dEx->getFollowingStar(sType);
		case parent			:
			return pkb->getChildren(sType);
		case parentStar		:
			return dEx->getChildrenStar(sType);
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
		default				:
			std::vector<Node*> empty;
			return empty;
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
			return dEx->getFollowedByStar(lineNum);
		case parent			:
			return pkb->getParent(lineNum);
		case parentStar		:
			return dEx->getParentsStar(lineNum);
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
		default				:
			std::vector<Node*> empty;
			return empty;
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
			return dEx->getFollowedByStar(statement);
		case parent			:
			return pkb->getParents(statement);
		case parentStar		:
			return dEx->getParentsStar(statement);
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
		default				:
			std::vector<Node*> empty;
			return empty;
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
			return dEx->getFollowedByStar(sType);
		case parent			:
			return pkb->getParents(sType);
		case parentStar		:
			return dEx->getParentsStar(sType);
		/*case calls			:
		case callsStar		:
		case next			:
		case nextStar		:
		case affects		:
		case affectsStar	:*/
		default				:
			std::vector<Node*> empty;
			return empty;
	}
}

ParamNode* QueryEvaluator::getOptimal(ParamNode* left, ParamNode* right) {
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

bool QueryEvaluator::resultNotEmpty(ParamNode* pNode, std::vector<Node*> nVec) {
	int symIndex = symbol->getIndex(pNode->getParam());
	std::vector<Node*> resultVec;
	for(std::vector<Node*>::iterator i = nVec.begin(); i != nVec.end(); i++) {
		if(pNode->getType() == (**i).getType()) {
			resultVec.push_back(*i);
		}
	}
	std::vector<Node*> symVec = symbol->getQuery().at(symIndex)->getPKBOutput();
	if(resultVec.size() == 0) {
		return false;
	} else if(symVec.size() == 0) {
		symbol->getQuery().at(symIndex)->setPKBOutput(resultVec);
	} else {
		std::vector<Node*> newVec;
		for(std::vector<Node*>::iterator i = resultVec.begin(); i != resultVec.end(); i++) {
			for(std::vector<Node*>::iterator j = symVec.begin(); j != symVec.end(); j++) {
				if(*i == *j) {
					newVec.push_back(*i);
					break;
				}
			}
		}
		symbol->getQuery().at(symIndex)->setPKBOutput(newVec);
	}
	return true;
}

void QueryEvaluator::evaluateResult() {
	std::string result;
	if(hasResult) {
		for(std::vector<Data*>::iterator i = symbol->getQuery().begin(); i != symbol->getQuery().end(); i++) {
			if((**i).getResult()) {
				result = getStringResult(*i);
				break;
			}
		}
		if(result.empty()) {
			result.append("true");
		}
	}
	std::cout<<result;
}

std::string QueryEvaluator::getStringResult(Data* sData) {
	std::string sResult;
	std::vector<Node*> nResult;
	if(sData->getPKBOutput().size() == 0) {
		nResult = pkb->getStatement(sData->getVarType());
	} else {
		nResult = sData->getPKBOutput();
	}
	std::ostringstream oss;
	oss << nResult.at(0)->getLine();
	sResult.append(oss.str());
	for(std::vector<Node*>::iterator i = nResult.begin() + 1; i != nResult.end(); i++){
		sResult.append(", ");
		oss << (**i).getLine();
		sResult.append(oss.str());
	}
	return sResult;
}