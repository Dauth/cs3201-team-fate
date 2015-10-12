#include "stdafx.h"
#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator(PKB* p) {
	pkb = p;
}

std::list<std::string> QueryEvaluator::evaluate(std::vector<ParamNode*> rVec, std::vector<QueryPart*> qVec) {
	resultSynonyms = rVec;
	queryParts = qVec;
	hasResult = true;
	
	queryWithNoResult.clear();
	queryWithOneResult.clear();
	queryWithTwoResults.clear();
	synonymVec.clear();
	resultTuples.clear();
	finalResult.clear();

	if(!queryParts.empty()) {
		optimise();

		if(hasResult) {
			evalQueryWithNoResult();
		}

		if(hasResult) {
			evalQueryWithOneResult();
		}

		if(hasResult) {
			evalQueryWithTwoResults();
		}
		
		evalFinalResult();
	}

	return finalResult;
}

void QueryEvaluator::optimise() {
	if(!resultSynonyms.empty()) {
		for(std::vector<ParamNode*>::iterator i = resultSynonyms.begin(); i != resultSynonyms.end(); i++) {
			synonymVec.push_back(new SynonymValues((**i).getParam()));
		}

		sortQueryParts();

		if(!hasResult) {
			return;
		}
	}
	
	for(std::vector<QueryPart*>::iterator i = queryParts.begin(); i != queryParts.end(); i++) {
		QueryType queryType = (**i).getType();
		ParamNode* left = (**i).getLeftParam();
		ParamNode* right = (**i).getRightParam();

		if((queryType == follows || queryType == followsStar || queryType == parent || queryType == parentStar) && left->getParam() == right->getParam()) {
			hasResult = false;
			return;
		}

		if((queryType == affects || queryType == affectsStar) && (left->getType() == whileLoop || left->getType() == ifelse || left->getType() == call || right->getType() == whileLoop || right->getType() == ifelse || right->getType() == call)) {
			hasResult = false;
			return;
		}

		if(queryType == with && left->getType() == right->getType()) {
			if(left->getParam() != right->getParam() && (left->getType() == integer || left->getType() == expression)) {
				hasResult = false;
				return;
			}
			else if(left->getParam() == right->getParam()) {
				queryParts.erase(i);
				i--;
			}
		}

		queryWithNoResult.push_back(*i);
	}
}

void QueryEvaluator::sortQueryParts() {
	int initSize = 1;
	int currSize = 0;

	while(currSize < initSize) {
		for(std::vector<QueryPart*>::iterator i = queryParts.begin(); i != queryParts.end(); i++) {
			initSize = queryParts.size();
			QueryType queryType = (**i).getType();
			ParamNode* left = (**i).getLeftParam();
			ParamNode* right = (**i).getRightParam();

			if((queryType == follows || queryType == followsStar || queryType == parent || queryType == parentStar) && left->getParam() == right->getParam()) {
				hasResult = false;
				return;
			}

			if((queryType == affects || queryType == affectsStar) && (left->getType() == whileLoop || left->getType() == ifelse || left->getType() == call || right->getType() == whileLoop || right->getType() == ifelse || right->getType() == call)) {
				hasResult = false;
				return;
			}

			if(queryType == with && left->getType() == right->getType()) {
				if(left->getParam() != right->getParam() && (left->getType() == integer || left->getType() == expression)) {
					hasResult = false;
					return;
				}
			
				queryParts.erase(i);
				i--;
			}

			if(left->getType() == integer || left->getType() == expression) {
				if(right->getType() == integer || right->getType() == expression) {
					queryWithNoResult.push_back(*i);

					queryParts.erase(i);
					i--;
				}
				else if(existsInSynVec(right->getParam())) {
					if(queryType == follows || queryType == parent || queryType == calls || queryType == next || queryType == with) {
						queryWithOneResult.insert(queryWithOneResult.begin(), *i);
					}
					else {
						queryWithOneResult.push_back(*i);
					}

					queryParts.erase(i);
					i--;
				}
			}
			else if((right->getType() == integer || right->getType() == expression) && existsInSynVec(left->getParam())) {
				if(queryType == follows || queryType == parent || queryType == calls || queryType == next || queryType == with) {
					queryWithOneResult.insert(queryWithOneResult.begin(), *i);
				}
				else {
					queryWithOneResult.push_back(*i);
				}

				queryParts.erase(i);
				i--;
			}
			else if(existsInSynVec(left->getParam())) {
				if(!existsInSynVec(right->getParam())) {
					synonymVec.push_back(new SynonymValues(right->getParam()));
				}

				if(queryType == follows || queryType == parent || queryType == calls || queryType == next || queryType == with) {
					queryWithTwoResults.insert(queryWithTwoResults.begin(), *i);
				}
				else {
					queryWithTwoResults.push_back(*i);
				}

				queryParts.erase(i);
				i--;
			}
			else if(existsInSynVec(right->getParam())) {
				synonymVec.push_back(new SynonymValues(left->getParam()));

				if(queryType == follows || queryType == parent || queryType == calls || queryType == next || queryType == with) {
					queryWithTwoResults.insert(queryWithTwoResults.begin(), *i);
				}
				else {
					queryWithTwoResults.push_back(*i);
				}

				queryParts.erase(i);
				i--;
			}

			currSize = queryParts.size();
		}
	}
}

bool QueryEvaluator::existsInSynVec(std::string name) {
	for(std::vector<SynonymValues*>::iterator i = synonymVec.begin(); i != synonymVec.end(); i++) {
		if(name == (**i).getName()) {
			return true;
		}
	}
	return false;
}

void QueryEvaluator::evalQueryWithNoResult() {
	for(std::vector<QueryPart*>::iterator i = queryWithNoResult.begin(); i != queryWithNoResult.end(); i++) {
		std::vector<std::pair<std::string, std::string>> result = getResult(*i);

		if(result.empty()) {
			hasResult = false;
			return;
		}
	}
}

void QueryEvaluator::evalQueryWithOneResult() {
	for(std::vector<QueryPart*>::iterator i = queryWithOneResult.begin(); i != queryWithOneResult.end(); i++) {
		std::vector<std::pair<std::string, std::string>> result = getResult(*i);

		if(result.empty()) {
			hasResult = false;
			return;
		}

		if((**i).getRightParam()->getType() != integer && (**i).getRightParam()->getType() != expression) {
			updateSynVal(NULL, (**i).getRightParam(), result);
		}
		else {
			updateSynVal((**i).getLeftParam(), NULL, result);
		}
	}
}

void QueryEvaluator::evalQueryWithTwoResults() {
	for(std::vector<QueryPart*>::iterator i = queryWithTwoResults.begin(); i != queryWithTwoResults.end(); i++) {
		std::vector<std::pair<std::string, std::string>> result = getResult(*i);
		
		if(result.empty()) {
			hasResult = false;
			return;
		}

		updateSynVal((**i).getLeftParam(), (**i).getRightParam(), result);
	}
}

std::vector<std::pair<std::string, std::string>> QueryEvaluator::getResult(QueryPart* qp) {
	if(qp->getType() == with) {
		return evalWithQuery(qp);
	}

	ParamNode* left = qp->getLeftParam();
	ParamNode* right = qp->getRightParam();

	if(left->getType() == integer || left->getType() == expression) {
		if(right->getType() == integer || right->getType() == expression) {
			return getResultFromPKB(qp->getType(), left->getParam(), right->getParam());
		}
		else {
			return getResultFromPKB(qp->getType(), left->getParam(), right->getType());
		}
	}
	else if(right->getType() == integer || right->getType() == expression) {
		if(qp->getType() == pattern) {
			return pkb->searchWithPattern(left->getType(), right->getParam(), qp->getLastParam()->getParam());
		}
		else {
			return getResultFromPKB(qp->getType(), left->getType(), right->getParam());
		}
	}
	else {
		if(qp->getType() == pattern) {
			return pkb->searchWithPattern(left->getType(), "_", qp->getLastParam()->getParam());
		}
		else {
			return getResultFromPKB(qp->getType(), left->getType(), right->getType());
		}
	}
}

std::vector<std::pair<std::string, std::string>> QueryEvaluator::evalWithQuery(QueryPart* qp) {
	ParamNode* left = qp->getLeftParam();
	ParamNode* right = qp->getRightParam();
	std::vector<std::pair<std::string, std::string>> result;

	if(left->getType() == integer) {
		std::vector<Node*> nodes = pkb->getNodes(right->getParam());

		for(std::vector<Node*>::iterator i = nodes.begin(); i != nodes.end(); i++) {
			if((**i).getLine() == left->getParam()) {
				std::pair<std::string, std::string> pair;
				pair.first = "";
				pair.second = left->getParam();
				result.push_back(pair);
				break;
			}
		}
	}
	else if(left->getType() == expression) {
		std::vector<Node*> nodes = pkb->getNodes(right->getParam());

		for(std::vector<Node*>::iterator i = nodes.begin(); i != nodes.end(); i++) {
			std::string string;

			string = (**i).getValue();

			if(string == left->getParam()) {
				if(right->getType() == call) {
					string = (**i).getLine();
				}

				std::pair<std::string, std::string> pair;
				pair.first = "";
				pair.second = string;
				result.push_back(pair);
				break;
			}
		}
	}
	else if(right->getType() == integer) {
		std::vector<Node*> nodes = pkb->getNodes(left->getParam());

		for(std::vector<Node*>::iterator i = nodes.begin(); i != nodes.end(); i++) {
			if((**i).getLine() == right->getParam()) {
				std::pair<std::string, std::string> pair;
				pair.first = right->getParam();
				pair.second = "";
				result.push_back(pair);
				break;
			}
		}
	}
	else if(right->getType() == expression) {
		std::vector<Node*> nodes = pkb->getNodes(left->getParam());

		for(std::vector<Node*>::iterator i = nodes.begin(); i != nodes.end(); i++) {
			std::string string;

			string = (**i).getValue();

			if(string == right->getParam()) {
				if(left->getType() == call) {
					string = (**i).getLine();
				}

				std::pair<std::string, std::string> pair;
				pair.first = string;
				pair.second = "";
				result.push_back(pair);
				break;
			}
		}
	}
	else{
		std::vector<Node*> leftNodes = pkb->getNodes(left->getType());
		std::vector<Node*> rightNodes = pkb->getNodes(right->getType());

		if(left->getAttrType() == stringType) {
			for(std::vector<Node*>::iterator i = leftNodes.begin(); i != leftNodes.end(); i++) {
				for(std::vector<Node*>::iterator j = rightNodes.begin(); j != rightNodes.end(); j++) {
					if((**i).getValue() == (**j).getValue()) {
						std::pair<std::string, std::string> pair;

						if(left->getType() == call) {
							pair.first = (**i).getLine();
						}
						else {
							pair.first = (**i).getValue();
						}

						if(right->getType() == call) {
							pair.second = (**i).getLine();
						}
						else {
							pair.second = (**i).getValue();
						}

						result.push_back(pair);
					}
				}
			}
		}
		else {
			for(std::vector<Node*>::iterator i = leftNodes.begin(); i != leftNodes.end(); i++) {
				for(std::vector<Node*>::iterator j = rightNodes.begin(); j != rightNodes.end(); j++) {
					if((**i).getLine() == (**j).getLine()) {
						std::pair<std::string, std::string> pair;
						pair.first = (**i).getLine();
						pair.second = (**i).getLine();
						result.push_back(pair);
					}
				}
			}
		}
	}

	return result;
}

std::vector<std::pair<std::string, std::string>> QueryEvaluator::getResultFromPKB(QueryType type, std::string left, std::string right) {
	switch(type) {
		case modifies	:
			return pkb->getModifies(left, right);
		case uses		:
			return pkb->getUses(left, right);
		case follows	:
			return pkb->getFollows(left, right);
		case followsStar:
			return pkb->getFollowsStar(left, right);
		case parent		:
			return pkb->getParents(left, right);
		case parentStar	:
			return pkb->getParentsStar(left, right);
		case calls		:
			return pkb->getCalls(left, right);
		case callsStar	:
			return pkb->getCallsStar(left, right);
/*		case next		:
			return pkb->getNext(left, right);
		case nextStar	:
			return pkb->getNextStar(left, right);
		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);*/
		default			:
			std::vector<std::pair<std::string, std::string>> empty;
			return empty;
	}
}

std::vector<std::pair<std::string, std::string>> QueryEvaluator::getResultFromPKB(QueryType type, std::string left, SyntType right) {
	switch(type) {
		case modifies	:
			return pkb->getModifies(left, right);
		case uses		:
			return pkb->getUses(left, right);
		case follows	:
			return pkb->getFollows(left, right);
		case followsStar:
			return pkb->getFollowsStar(left, right);
		case parent		:
			return pkb->getParents(left, right);
		case parentStar	:
			return pkb->getParentsStar(left, right);
		case calls		:
			return pkb->getCalls(left, right);
		case callsStar	:
			return pkb->getCallsStar(left, right);
/*		case next		:
			return pkb->getNext(left, right);
		case nextStar	:
			return pkb->getNextStar(left, right);
		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);*/
		default			:
			std::vector<std::pair<std::string, std::string>> empty;
			return empty;
	}
}

std::vector<std::pair<std::string, std::string>> QueryEvaluator::getResultFromPKB(QueryType type, SyntType left, std::string right) {
	switch(type) {
		case modifies	:
			return pkb->getModifies(left, right);
		case uses		:
			return pkb->getUses(left, right);
		case follows	:
			return pkb->getFollows(left, right);
		case followsStar:
			return pkb->getFollowsStar(left, right);
		case parent		:
			return pkb->getParents(left, right);
		case parentStar	:
			return pkb->getParentsStar(left, right);
		case calls		:
			return pkb->getCalls(left, right);
		case callsStar	:
			return pkb->getCallsStar(left, right);
/*		case next		:
			return pkb->getNext(left, right);
		case nextStar	:
			return pkb->getNextStar(left, right);
		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);*/
		default			:
			std::vector<std::pair<std::string, std::string>> empty;
			return empty;
	}
}

std::vector<std::pair<std::string, std::string>> QueryEvaluator::getResultFromPKB(QueryType type, SyntType left, SyntType right) {
	switch(type) {
		case modifies	:
			return pkb->getModifies(left, right);
		case uses		:
			return pkb->getUses(left, right);
		case follows	:
			return pkb->getFollows(left, right);
		case followsStar:
			return pkb->getFollowsStar(left, right);
		case parent		:
			return pkb->getParents(left, right);
		case parentStar	:
			return pkb->getParentsStar(left, right);
		case calls		:
			return pkb->getCalls(left, right);
		case callsStar	:
			return pkb->getCallsStar(left, right);
/*		case next		:
			return pkb->getNext(left, right);
		case nextStar	:
			return pkb->getNextStar(left, right);
		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);*/
		default			:
			std::vector<std::pair<std::string, std::string>> empty;
			return empty;
	}
}

void QueryEvaluator::updateSynVal(ParamNode* lNode, ParamNode* rNode, std::vector<std::pair<std::string, std::string>> vec) {
	if(lNode != NULL && rNode != NULL) {
		SynonymValues* leftSyn = getSynVal(lNode->getParam());
		SynonymValues* rightSyn = getSynVal(rNode->getParam());
		std::set<std::string> leftVal = leftSyn->getValues();
		std::set<std::string> rightVal = rightSyn->getValues();

		if(!leftVal.empty()) {
			for(std::vector<std::pair<std::string, std::string>>::iterator i = vec.begin(); i != vec.end(); i++) {
				if(leftVal.find(i->first) == leftVal.end()) {
					vec.erase(i);
					i--;
				}
			}
		}

		if(!rightVal.empty()) {
			for(std::vector<std::pair<std::string, std::string>>::iterator i = vec.begin(); i != vec.end(); i++) {
				if(rightVal.find(i->second) == rightVal.end()) {
					vec.erase(i);
					i--;
				}
			}
		}

		if(vec.empty()) {
			hasResult = false;
			return;
		}

		std::set<std::string> newLeftVal;
		std::set<std::string> newRightVal;

		for(std::vector<std::pair<std::string, std::string>>::iterator i = vec.begin(); i != vec.end(); i++) {
			newLeftVal.insert(i->first);
			newRightVal.insert(i->second);
		}

		if(leftVal.empty()) {
			leftSyn->setValues(newLeftVal);
		}
		else if(newLeftVal.size() < leftVal.size()) {
			leftSyn->setValues(newLeftVal);
			updateRelatedSynVal(leftSyn);
		}

		if(rightVal.empty()) {
			rightSyn->setValues(newRightVal);
		}
		else if(newRightVal.size() < rightVal.size()) {
			rightSyn->setValues(newRightVal);
			updateRelatedSynVal(rightSyn);
		}

		if(!hasResult) {
			return;
		}
		
		std::vector<std::pair<std::string, std::string>> tuples;
		std::pair<std::string, std::string> pair;
		pair.first = leftSyn->getName();
		pair.second = rightSyn->getName();
		tuples.push_back(pair);

		for(std::vector<std::pair<std::string, std::string>>::iterator i = vec.begin(); i != vec.end(); i++) {
			tuples.push_back(*i);
		}

		resultTuples.push_back(&tuples);
	}
	else if(lNode != NULL) {
		SynonymValues* leftSyn = getSynVal(lNode->getParam());
		std::set<std::string> leftVal = leftSyn->getValues();

		if(!leftVal.empty()) {
			for(std::vector<std::pair<std::string, std::string>>::iterator i = vec.begin(); i != vec.end(); i++) {
				if(leftVal.find(i->first) == leftVal.end()) {
					vec.erase(i);
					i--;
				}
			}
		}

		if(vec.empty()) {
			hasResult = false;
			return;
		}

		std::set<std::string> newLeftVal;

		for(std::vector<std::pair<std::string, std::string>>::iterator i = vec.begin(); i != vec.end(); i++) {
			newLeftVal.insert(i->first);
		}

		if(leftVal.empty()) {
			leftSyn->setValues(newLeftVal);
		}
		else if(newLeftVal.size() < leftVal.size()) {
			leftSyn->setValues(newLeftVal);
			updateRelatedSynVal(leftSyn);
		}
	}
	else {
		SynonymValues* rightSyn = getSynVal(rNode->getParam());
		std::set<std::string> rightVal = rightSyn->getValues();

		if(!rightVal.empty()) {
			for(std::vector<std::pair<std::string, std::string>>::iterator i = vec.begin(); i != vec.end(); i++) {
				if(rightVal.find(i->second) == rightVal.end()) {
					vec.erase(i);
					i--;
				}
			}
		}

		if(vec.empty()) {
			hasResult = false;
			return;
		}

		std::set<std::string> newRightVal;

		for(std::vector<std::pair<std::string, std::string>>::iterator i = vec.begin(); i != vec.end(); i++) {
			newRightVal.insert(i->second);
		}

		if(rightVal.empty()) {
			rightSyn->setValues(newRightVal);
		}
		else if(newRightVal.size() < rightVal.size()) {
			rightSyn->setValues(newRightVal);
			updateRelatedSynVal(rightSyn);
		}
	}
}

void QueryEvaluator::updateRelatedSynVal(SynonymValues* synVal) {
	std::set<std::string> valSet = synVal->getValues();

	for(std::vector<std::vector<std::pair<std::string, std::string>>*>::iterator i = resultTuples.begin(); i != resultTuples.end(); i++) {
		int initSize = (**i).size();

		if((**i).begin()->first == synVal->getName()) {
			for(std::vector<std::pair<std::string, std::string>>::iterator j = (**i).begin() + 1; j != (**i).end(); j++) {
				if(valSet.find(j->first) == valSet.end()) {
					(**i).erase(j);
					j--;
				}
			}

			if((**i).begin() + 1 == (**i).end()) {
				hasResult = false;
				return;
			}

			if((**i).size() < initSize) {
				std::set<std::string> rightVal;

				for(std::vector<std::pair<std::string, std::string>>::iterator j = (**i).begin() + 1; j != (**i).end(); j++) {
					rightVal.insert(j->second);
				}

				SynonymValues* rightSyn = getSynVal((**i).begin()->second);
				rightSyn->setValues(rightVal);
				updateRelatedSynVal(rightSyn);
			}
		}
		else if((**i).begin()->second == synVal->getName()) {
			for(std::vector<std::pair<std::string, std::string>>::iterator j = (**i).begin() + 1; j != (**i).end(); j++) {
				if(valSet.find(j->second) == valSet.end()) {
					(**i).erase(j);
					j--;
				}
			}

			if((**i).begin() + 1 == (**i).end()) {
				hasResult = false;
				return;
			}

			if((**i).size() < initSize) {
				std::set<std::string> leftVal;

				for(std::vector<std::pair<std::string, std::string>>::iterator j = (**i).begin() + 1; j != (**i).end(); j++) {
					leftVal.insert(j->first);
				}

				SynonymValues* leftSyn = getSynVal((**i).begin()->first);
				leftSyn->setValues(leftVal);
				updateRelatedSynVal(leftSyn);
			}
		}

		if(!hasResult) {
			return;
		}
	}
}

void QueryEvaluator::evalFinalResult() {
	if(resultSynonyms.empty()) {
		if(hasResult) {
			finalResult.push_back("true");
		}
		else {
			finalResult.push_back("false");
		}
	}
	else if(hasResult) {
		formFinalResult("", 0);
	}
}

void QueryEvaluator::formFinalResult(std::string s, int index) {
	ParamNode* node = resultSynonyms[index];
	SynonymValues* synVal = getSynVal(node->getParam());
	std::set<std::string> valSet = synVal->getValues();
	index++;

	if(valSet.empty()) {
		std::vector<Node*> result = pkb->getNodes(resultSynonyms[index]->getType());

		for(std::vector<Node*>::iterator i = result.begin(); i != result.end(); i++) {
			if((node->getType() == call && node->getAttrType() == stringType) || node->getType() == procedure || node->getType() == variable || node->getType() == constant) {
				for(std::vector<Node*>::iterator i = result.begin(); i != result.end(); i++) {
					valSet.insert((**i).getValue());
				}
			}
			else {
				for(std::vector<Node*>::iterator i = result.begin(); i != result.end(); i++) {
					valSet.insert((**i).getLine());
				}
			}
		}
	}
	else if(resultSynonyms[index]->getType() == call && resultSynonyms[index]->getAttrType() == stringType) {
		std::vector<Node*> result = pkb->getNodes(resultSynonyms[index]->getType());

		for(std::vector<Node*>::iterator i = result.begin(); i != result.end(); i++) {
			std::string stmtLine = (**i).getLine();

			if(valSet.find(stmtLine) != valSet.end()) {
				valSet.insert((**i).getValue());
				valSet.erase(stmtLine);
			}
		}
	}

	for(std::set<std::string>::iterator i = valSet.begin(); i != valSet.end(); i++) {
		std::string singleResult = s;
		singleResult.append(*i);
		singleResult.append(" ");

		if(index < resultSynonyms.size()) {
			formFinalResult(singleResult, index);
		}
		else {
			finalResult.push_back(singleResult);
		}
	}
}

SynonymValues* QueryEvaluator::getSynVal(std::string name) {

	for(std::vector<SynonymValues*>::iterator i = synonymVec.begin(); i != synonymVec.end(); i++) {
		if(name == (**i).getName()) {
			return *i;
		}
	}

	return NULL;
}