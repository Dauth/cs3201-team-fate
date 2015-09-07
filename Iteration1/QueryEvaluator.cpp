#include "stdafx.h"
#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator(PKB* p, DesignExtractor* de) {
	pkb = p;
	dEx = de;
}

void QueryEvaluator::evaluate(Symbol* s, QueryTree* qt) {
	sc = new SymbolConnection();
	hasResult = true;
	symbol = s;
	tree = qt;
	evaluatePattern();
	evaluateQuery();
	evaluateResult();
}

void QueryEvaluator::evaluatePattern() {
	PatternNode* pattern = tree->getPattern();
	while(pattern) {
		evaluateSinglePattern(pattern);
		pattern = tree->getPattern();
	}
}

void QueryEvaluator::evaluateSinglePattern(PatternNode* p) {
	std::vector<Node*> tempResult;
	if(p->getMiddleParam()->getType() == expression) {
		tempResult = dEx->searchWithPattern(p->getLeftParam()->getType(), p->getMiddleParam()->getParam(), p->getRightParam()->getParam());
	} else {
		std::vector<Node*> var;
		int index = symbol->getIndex(p->getMiddleParam()->getParam());
		if(symbol->getQuery().at(index)->getPKBOutput().empty()) {
			tempResult = dEx->searchWithPattern(p->getLeftParam()->getType(), "_", p->getRightParam()->getParam());
		} else {
			std::set<Node*> temp;
			std::vector<Node*> varVec = symbol->getQuery().at(index)->getPKBOutput();
			for(std::vector<Node*>::iterator i = varVec.begin(); i != varVec.end(); i++) {
				tempResult = dEx->searchWithPattern(p->getLeftParam()->getType(), (**i).getVariable()->getName(), p->getRightParam()->getParam());
				for(std::vector<Node*>::iterator j = tempResult.begin(); j != tempResult.end(); j++){
				temp.insert(*j);
			}
		}
		tempResult.clear();
		for(std::set<Node*>::iterator i = temp.begin(); i != temp.end(); i++) {
			tempResult.push_back(*i);
			}
		}
		var = getVarFromPattern(tempResult);
		hasResult = resultNotEmpty(p->getMiddleParam(), var);
		if(!tempResult.empty()) {
			sc->addPattern(p);
		}
	}
	hasResult = resultNotEmpty(p->getLeftParam(), tempResult);
}

void QueryEvaluator::evaluateQuery() {
	QueryNode* query = tree->getQuery();
	while(query) {
		evaluateSingleQuery(query);
		query = tree->getQuery();
	}
}

void QueryEvaluator::evaluateSingleQuery(QueryNode* q) {
	ParamNode* left = q->getLeftParam();
	ParamNode* right = q->getRightParam();
	ParamNode* arg = getOptimal(left, right);
	if(left == arg) {
		hasResult = evaluateRight(right, q->getType(), left);
		if(hasResult && left->getType() != integer && left->getType() != expression) {
			hasResult = evaluateLeft(left, q->getType(), right);
			sc->addQuery(q);
		}
	} else {
		hasResult = evaluateLeft(left, q->getType(), right);
		if(hasResult && right->getType() != integer && right->getType() != expression) {
			hasResult = evaluateRight(right, q->getType(), left);
			sc->addQuery(q);
		}
	}
}

bool QueryEvaluator::evaluateLeft(ParamNode* rNode, query_type type, ParamNode* lNode) {
	std::vector<Node*> result;
	if(lNode->getType() == integer) {
		result = evaluateLeftByType(type, std::stoi(lNode->getParam()));
	} else if(lNode->getType() == expression) {
		result = evaluateLeftByType(type, lNode->getParam());
	} else {
		int index = symbol->getIndex(lNode->getParam());
		std::vector<Node*> varVec = symbol->getQuery().at(index)->getPKBOutput();
		if(varVec.empty()) {
			result = evaluateLeftByType(type, lNode->getType());
		} else {
			std::set<Node*> temp;
			for(int i = 0; i < varVec.size(); i++) {
				if(varVec.at(i)->getType() == variable) {
					result = evaluateLeftByType(type, varVec.at(i)->getVariable()->getName());
				} else {
					result = evaluateLeftByType(type, varVec.at(i)->getLine());
				}
				for(int j = 0; j < result.size(); j++) {
					temp.insert(result.at(j));
				}
			}
			result.clear();
			for(std::set<Node*>::iterator i = temp.begin(); i != temp.end(); i++) {
				result.push_back(*i);
			}
		}
	}
	return resultNotEmpty(rNode, result);
}

bool QueryEvaluator::evaluateRight(ParamNode* lNode, query_type type, ParamNode* rNode) {
	std::vector<Node*> result;
	if(rNode->getType() == integer) {
		result = evaluateRightByType(type, std::stoi(rNode->getParam()));
	} else if(rNode->getType() == expression) {
		result = evaluateRightByType(type, rNode->getParam());
	} else {
		int index = symbol->getIndex(rNode->getParam());
		std::vector<Node*> varVec = symbol->getQuery().at(index)->getPKBOutput();
		if(varVec.empty()) {
			result = evaluateRightByType(type, rNode->getType());
		} else {
			std::set<Node*> temp;
			for(int i = 0; i < varVec.size(); i++) {
				if(varVec.at(i)->getType() == variable) {
					result = evaluateRightByType(type, varVec.at(i)->getVariable()->getName());
				} else {
					result = evaluateRightByType(type, varVec.at(i)->getLine());
				}
				for(int j = 0; j < result.size(); j++) {
					temp.insert(result.at(j));
				}
			}
			result.clear();
			for(std::set<Node*>::iterator i = temp.begin(); i != temp.end(); i++) {
				result.push_back(*i);
			}
		}
	}
	return resultNotEmpty(lNode, result);
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

std::vector<Node*> QueryEvaluator::getVarFromPattern(std::vector<Node*> nVec) {
	std::vector<Node*> vVec;
	for(std::vector<Node*>::iterator i = nVec.begin(); i != nVec.end(); ++i) {
		vVec.push_back((**i).getLeftChild());

	}
	return vVec;
}

bool QueryEvaluator::resultNotEmpty(ParamNode* pNode, std::vector<Node*> nVec) {
	int symIndex = symbol->getIndex(pNode->getParam());
	std::vector<Node*> resultVec;

	//get all Node* with same type as pNode
	for(std::vector<Node*>::iterator i = nVec.begin(); i != nVec.end(); ++i) {
		if(pNode->getType() == statement && ((**i).getType() == whileLoop || (**i).getType() == ifelse || (**i).getType() == assignment || (**i).getType() == call)) {
			resultVec.push_back(*i);
		} else if(pNode->getType() == (**i).getType()) {
			resultVec.push_back(*i);
		}
	}

	std::vector<Node*> symVec = symbol->getQuery().at(symIndex)->getPKBOutput();

	//intersect resultVec and symVec
	if(resultVec.size() == 0) {
		return false;
	} else {
		if(symVec.size() == 0) {
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
		if(symVec.size() > symbol->getQuery().at(symIndex)->getPKBOutput().size()) {
			changePartners(pNode);
		}
	}
	return true;
}

void QueryEvaluator::changePartners(ParamNode* pNode) {
	std::vector<PatternNode*> pVec = sc->getPatterns(pNode->getParam());
	for(std::vector<PatternNode*>::iterator i = pVec.begin(); i != pVec.end(); i++) {
		evaluateSinglePattern(*i);
	}
	std::vector<QueryNode*> qVec = sc->getQueries(pNode->getParam());
	for(std::vector<QueryNode*>::iterator i = qVec.begin(); i != qVec.end(); i++) {
		evaluateSingleQuery(*i);
	}
}

void QueryEvaluator::evaluateResult() {
	std::string result;
	if(hasResult) {
		std::vector<Data*> symData = symbol->getQuery();
		for(std::vector<Data*>::iterator i = symData.begin(); i != symData.end(); i++) {
			if((**i).getResult()) {
				result = getStringResult(*i);
				break;
			}
		}
		if(result.empty()) {
			result = "true";
		}
	}
	std::cout<<result;
}

std::string QueryEvaluator::getStringResult(Data* sData) {
	std::string sResult;
	std::vector<Node*> nResult;
	if(sData->getPKBOutput().size() == 0) {
		nResult = pkb->getNodes(sData->getVarType());
	} else {
		nResult = sData->getPKBOutput();
	}
	if(sData->getVarType() != variable) {
		std::set<int> sortedRes;
		for(std::vector<Node*>::iterator i = nResult.begin(); i != nResult.end(); i++){
			sortedRes.insert((**i).getLine());
		}
		if(!sortedRes.empty()) {
			std::ostringstream oss;
			std::set<int>::iterator i = sortedRes.begin();
			oss << *i;
			for(i++; i != sortedRes.end(); i++){
				oss << ", ";
				oss << *i;
			}
			sResult.append(oss.str());
		}
	} else {
		std::set<std::string> sortedRes;
		for(std::vector<Node*>::iterator i = nResult.begin(); i != nResult.end(); i++){
			sortedRes.insert((**i).getVariable()->getName());
		}
		if(!sortedRes.empty()) {
			std::set<std::string>::iterator i = sortedRes.begin();
			sResult.append(*i);
			for(i++; i != sortedRes.end(); i++){
				sResult.append(", ");
				sResult.append(*i);
			}
		}
	}
	return sResult;
}