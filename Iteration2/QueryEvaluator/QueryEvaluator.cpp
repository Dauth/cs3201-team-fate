#include "stdafx.h"
#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator(PKB* p) {
	pkb = p;
}

list<string> QueryEvaluator::evaluate(vector<ParamNode*> rVec, vector<QueryPart*> qVec) {
	resultSynonyms = rVec;
	queryParts = qVec;
	hasResult = true;
	
	queryWithNoResult.clear();
	queryWithOneResult.clear();
	queryWithTwoResults.clear();
	synonymVec.clear();
	resultTuples.clear();
	finalResult.clear();
	optimise();

	if(!queryParts.empty()) {

		if(hasResult) {
			evalQueryWithNoResult();
		}

		if(hasResult) {
			evalQueryWithOneResult();
		}

		if(hasResult) {
			evalQueryWithTwoResults();
		}
	}
		
	evalFinalResult();
	return finalResult;
}

void QueryEvaluator::optimise() {
	if(!resultSynonyms.empty()) {
		for(unsigned int i = 0; i < resultSynonyms.size(); i++) {
			synonymVec.push_back(new SynonymValues(resultSynonyms[i]->getParam()));
		}

		sortQueryParts();

		if(!hasResult) {
			return;
		}
	}
	
	for(unsigned int i = 0; i < queryParts.size(); i++) {
		QueryType queryType = queryParts[i]->getType();
		ParamNode* left = queryParts[i]->getLeftParam();
		ParamNode* right = queryParts[i]->getRightParam();

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
				queryParts.erase(queryParts.begin() + i);
				i--;
			}
		}

		queryWithNoResult.push_back(queryParts[i]);
	}
}

void QueryEvaluator::sortQueryParts() {
	int initSize = 1;
	int currSize = 0;

	while(currSize < initSize) {
		initSize = queryParts.size();

		for(unsigned int i = 0; i < queryParts.size(); i++) {
			QueryType queryType = queryParts[i]->getType();
			ParamNode* left = queryParts[i]->getLeftParam();
			ParamNode* right = queryParts[i]->getRightParam();

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
			
				queryParts.erase(queryParts.begin() + i);
				i--;
			}

			if(left->getType() == integer || left->getType() == expression) {
				if(right->getType() == integer || right->getType() == expression) {
					queryWithNoResult.push_back(queryParts[i]);

					queryParts.erase(queryParts.begin() + i);
					i--;
				}
				else if(existsInSynVec(right->getParam())) {
					if(queryType == follows || queryType == parent || queryType == calls || queryType == nxt || queryType == with) {
						queryWithOneResult.insert(queryWithOneResult.begin(), queryParts[i]);
					}
					else {
						queryWithOneResult.push_back(queryParts[i]);
					}

					queryParts.erase(queryParts.begin() + i);
					i--;
				}
			}
			else if((right->getType() == integer || right->getType() == expression) && existsInSynVec(left->getParam())) {
				if(queryType == follows || queryType == parent || queryType == calls || queryType == nxt || queryType == with) {
					queryWithOneResult.insert(queryWithOneResult.begin(), queryParts[i]);
				}
				else {
					queryWithOneResult.push_back(queryParts[i]);
				}

				queryParts.erase(queryParts.begin() + i);
				i--;
			}
			else if(existsInSynVec(left->getParam())) {
				if(!existsInSynVec(right->getParam())) {
					synonymVec.push_back(new SynonymValues(right->getParam()));
				}

				if(queryType == follows || queryType == parent || queryType == calls || queryType == nxt || queryType == with) {
					queryWithTwoResults.insert(queryWithTwoResults.begin(), queryParts[i]);
				}
				else {
					queryWithTwoResults.push_back(queryParts[i]);
				}

				queryParts.erase(queryParts.begin() + i);
				i--;
			}
			else if(existsInSynVec(right->getParam())) {
				synonymVec.push_back(new SynonymValues(left->getParam()));

				if(queryType == follows || queryType == parent || queryType == calls || queryType == nxt || queryType == with) {
					queryWithTwoResults.insert(queryWithTwoResults.begin(), queryParts[i]);
				}
				else {
					queryWithTwoResults.push_back(queryParts[i]);
				}

				queryParts.erase(queryParts.begin() + i);
				i--;
			}

			currSize = queryParts.size();
		}
	}
}

bool QueryEvaluator::existsInSynVec(string name) {
	for(unsigned int i = 0; i < synonymVec.size(); i++) {
		if(name == synonymVec[i]->getName()) {
			return true;
		}
	}
	return false;
}

void QueryEvaluator::evalQueryWithNoResult() {
	for(unsigned int i = 0; i < queryWithNoResult.size(); i++) {
		vector<pair<string, string>> result = getResult(queryWithNoResult[i]);

		if(result.empty()) {
			hasResult = false;
			return;
		}
	}
}

void QueryEvaluator::evalQueryWithOneResult() {
	for(unsigned int i = 0; i < queryWithOneResult.size(); i++) {
		vector<pair<string, string>> result = getResult(queryWithOneResult[i]);

		if(result.empty()) {
			hasResult = false;
			return;
		}

		if(queryWithOneResult[i]->getRightParam()->getType() != integer && queryWithOneResult[i]->getRightParam()->getType() != expression) {
			updateSynVal(NULL, queryWithOneResult[i]->getRightParam(), result);
		}
		else {
			updateSynVal(queryWithOneResult[i]->getLeftParam(), NULL, result);
		}
	}
}

void QueryEvaluator::evalQueryWithTwoResults() {
	for(unsigned int i = 0; i < queryWithTwoResults.size(); i++) {
		vector<pair<string, string>> result = getResult(queryWithTwoResults[i]);
		
		if(result.empty()) {
			hasResult = false;
			return;
		}
		
		updateSynVal(queryWithTwoResults[i]->getLeftParam(), queryWithTwoResults[i]->getRightParam(), result);
	}
}

vector<pair<string, string>> QueryEvaluator::getResult(QueryPart* qp) {
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

vector<pair<string, string>> QueryEvaluator::evalWithQuery(QueryPart* qp) {
	ParamNode* left = qp->getLeftParam();
	ParamNode* right = qp->getRightParam();
	vector<pair<string, string>> result;

	if(left->getType() == integer && right->getType() != constant) {
		vector<Node*> nodes = pkb->getNodes(right->getType());

		for(unsigned int i = 0; i < nodes.size(); i++) {
			if(nodes[i]->getLine() == left->getParam()) {
				pair<string, string> p;
				p.first = "";
				p.second = nodes[i]->getLine();
				result.push_back(p);
				break;
			}
		}
	}
	else if(left->getType() == integer) {
		vector<Node*> nodes = pkb->getNodes(right->getType());

		for(unsigned int i = 0; i < nodes.size(); i++) {
			if(nodes[i]->getValue() == left->getParam()) {
				pair<string, string> p;
				p.first = "";
				p.second = nodes[i]->getValue();
				result.push_back(p);
				break;
			}
		}
	}
	else if(left->getType() == expression) {
		vector<Node*> nodes = pkb->getNodes(right->getType());

		for(unsigned int i = 0; i < nodes.size(); i++) {
			string value;

			value = nodes[i]->getValue();

			if(value == left->getParam()) {
				if(right->getType() == call) {
					value = nodes[i]->getLine();
				}

				pair<string, string> p;
				p.first = "";
				p.second = value;
				result.push_back(p);
				break;
			}
		}
	}
	else if(right->getType() == integer && left->getType() != constant) {
		vector<Node*> nodes = pkb->getNodes(left->getType());

		for(unsigned int i = 0; i < nodes.size(); i++) {
			if(nodes[i]->getLine() == right->getParam()) {
				pair<string, string> p;
				p.first = nodes[i]->getLine();
				p.second = "";
				result.push_back(p);
				break;
			}
		}
	}
	else if(right->getType() == integer) {
		vector<Node*> nodes = pkb->getNodes(left->getType());

		for(unsigned int i = 0; i < nodes.size(); i++) {
			if(nodes[i]->getValue() == right->getParam()) {
				pair<string, string> p;
				p.first = nodes[i]->getValue();
				p.second = "";
				result.push_back(p);
				break;
			}
		}
	}
	else if(right->getType() == expression) {
		vector<Node*> nodes = pkb->getNodes(left->getType());

		for(unsigned int i = 0; i < nodes.size(); i++) {
			string value;

			value = nodes[i]->getValue();

			if(value == right->getParam()) {
				if(left->getType() == call) {
					value = nodes[i]->getLine();
				}

				pair<string, string> p;
				p.first = value;
				p.second = "";
				result.push_back(p);
				break;
			}
		}
	}
	else{
		vector<Node*> leftNodes = pkb->getNodes(left->getType());
		vector<Node*> rightNodes = pkb->getNodes(right->getType());

		if(left->getAttrType() == stringType) {
			for(unsigned int i = 0; i < leftNodes.size(); i++) {
				for(unsigned int j = 0; j < rightNodes.size(); j++) {
					if(leftNodes[i]->getValue() == rightNodes[j]->getValue()) {
						pair<string, string> p;

						if(left->getType() == call) {
							p.first = leftNodes[i]->getLine();
						}
						else {
							p.first = leftNodes[i]->getValue();
						}

						if(right->getType() == call) {
							p.second = rightNodes[j]->getLine();
						}
						else {
							p.second = rightNodes[j]->getValue();
						}

						result.push_back(p);
					}
				}
			}
		}
		else if(left->getType() == constant && right->getType() == constant) {
			for(unsigned int i = 0; i < leftNodes.size(); i++) {
				for(unsigned int j = 0; j < rightNodes.size(); j++) {
					if(leftNodes[i]->getValue() == rightNodes[j]->getValue()) {
						pair<string, string> p;
						p.first = leftNodes[i]->getValue();
						p.second = rightNodes[j]->getValue();
						result.push_back(p);
					}
				}
			}
		}
		else if(left->getType() == constant) {
			for(unsigned int i = 0; i < leftNodes.size(); i++) {
				for(unsigned int j = 0; j < rightNodes.size(); j++) {
					if(leftNodes[i]->getValue() == rightNodes[j]->getLine()) {
						pair<string, string> p;
						p.first = leftNodes[i]->getValue();
						p.second = rightNodes[j]->getLine();
						result.push_back(p);
					}
				}
			}
		}
		else if(right->getType() == constant) {
			for(unsigned int i = 0; i < leftNodes.size(); i++) {
				for(unsigned int j = 0; j < rightNodes.size(); j++) {
					if(leftNodes[i]->getLine() == rightNodes[j]->getValue()) {
						pair<string, string> p;
						p.first = leftNodes[i]->getLine();
						p.second = rightNodes[j]->getValue();
						result.push_back(p);
					}
				}
			}
		}
		else {
			for(unsigned int i = 0; i < leftNodes.size(); i++) {
				for(unsigned int j = 0; j < rightNodes.size(); j++) {
					if(leftNodes[i]->getLine() == rightNodes[j]->getLine()) {
						pair<string, string> p;
						p.first = leftNodes[i]->getLine();
						p.second = rightNodes[j]->getLine();
						result.push_back(p);
					}
				}
			}
		}
	}

	return result;
}

vector<pair<string, string>> QueryEvaluator::getResultFromPKB(QueryType type, string left, string right) {
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
		case nxt		:
			return pkb->getNext(left, right);
		case nxtStar	:
			return pkb->getNextStar(left, right);
/*		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);*/
		default			:
			vector<pair<string, string>> empty;
			return empty;
	}
}

vector<pair<string, string>> QueryEvaluator::getResultFromPKB(QueryType type, string left, SyntType right) {
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
		case nxt		:
			return pkb->getNext(left, right);
		case nxtStar	:
			return pkb->getNextStar(left, right);
/*		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);*/
		default			:
			vector<pair<string, string>> empty;
			return empty;
	}
}

vector<pair<string, string>> QueryEvaluator::getResultFromPKB(QueryType type, SyntType left, string right) {
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
		case nxt		:
			return pkb->getNext(left, right);
		case nxtStar	:
			return pkb->getNextStar(left, right);
/*		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);*/
		default			:
			vector<pair<string, string>> empty;
			return empty;
	}
}

vector<pair<string, string>> QueryEvaluator::getResultFromPKB(QueryType type, SyntType left, SyntType right) {
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
		case nxt		:
			return pkb->getNext(left, right);
		case nxtStar	:
			return pkb->getNextStar(left, right);
/*		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);*/
		default			:
			vector<pair<string, string>> empty;
			return empty;
	}
}

void QueryEvaluator::updateSynVal(ParamNode* lNode, ParamNode* rNode, vector<pair<string, string>> vec) {
	if(lNode != NULL && rNode != NULL) {
		SynonymValues* leftSyn = getSynVal(lNode->getParam());
		SynonymValues* rightSyn = getSynVal(rNode->getParam());
		set<string> leftVal = leftSyn->getValues();
		set<string> rightVal = rightSyn->getValues();

		if(!leftVal.empty()) {
			for(unsigned int i = 0; i != vec.size(); i++) {
				if(leftVal.find(vec[i].first) == leftVal.end()) {
					vec.erase(vec.begin() + i);
					i--;
				}
			}
		}

		if(!rightVal.empty()) {
			for(unsigned int i = 0; i < vec.size(); i++) {
				if(rightVal.find(vec[i].second) == rightVal.end()) {
					vec.erase(vec.begin() + i);
					i--;
				}
			}
		}

		if(vec.empty()) {
			hasResult = false;
			return;
		}

		set<string> newLeftVal;
		set<string> newRightVal;

		for(unsigned int i = 0; i < vec.size(); i++) {
			newLeftVal.insert(vec[i].first);
			newRightVal.insert(vec[i].second);
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
		
		vector<pair<string, string>> tuples;
		pair<string, string> p;
		p.first = leftSyn->getName();
		p.second = rightSyn->getName();
		tuples.push_back(p);

		for(unsigned int i = 0; i < vec.size(); i++) {
			tuples.push_back(vec[i]);
		}

		resultTuples.push_back(&tuples);
	}
	else if(lNode != NULL) {
		SynonymValues* leftSyn = getSynVal(lNode->getParam());
		set<string> leftVal = leftSyn->getValues();

		if(!leftVal.empty()) {
			for(unsigned int i = 0; i < vec.size(); i++) {
				if(leftVal.find(vec[i].first) == leftVal.end()) {
					vec.erase(vec.begin() + i);
					i--;
				}
			}
		}

		if(vec.empty()) {
			hasResult = false;
			return;
		}

		set<string> newLeftVal;

		for(unsigned int i = 0; i < vec.size(); i++) {
			newLeftVal.insert(vec[i].first);
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
		set<string> rightVal = rightSyn->getValues();

		if(!rightVal.empty()) {
			for(unsigned int i = 0; i < vec.size(); i++) {
				if(rightVal.find(vec[i].second) == rightVal.end()) {
					vec.erase(vec.begin() + i);
					i--;
				}
			}
		}

		if(vec.empty()) {
			hasResult = false;
			return;
		}

		set<string> newRightVal;

		for(unsigned int i = 0; i < vec.size(); i++) {
			newRightVal.insert(vec[i].second);
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
	set<string> valSet = synVal->getValues();

	for(unsigned int i = 0; i < resultTuples.size(); i++) {
		unsigned int initSize = resultTuples[i]->size();

		if(resultTuples[i]->begin()->first == synVal->getName()) {
			for(unsigned int j = 1; j != resultTuples[i]->size(); j++) {
				if(valSet.find(resultTuples[i]->at(j).first) == valSet.end()) {
					resultTuples[i]->erase(resultTuples[i]->begin() + j);
					j--;
				}
			}

			if(resultTuples[i]->size() == 1) {
				hasResult = false;
				return;
			}

			if(resultTuples[i]->size() < initSize) {
				set<string> rightVal;

				for(unsigned int j = 1; j < resultTuples[i]->size(); j++) {
					rightVal.insert(resultTuples[i]->at(j).second);
				}

				SynonymValues* rightSyn = getSynVal(resultTuples[i]->begin()->second);
				rightSyn->setValues(rightVal);
				updateRelatedSynVal(rightSyn);
			}
		}
		else if(resultTuples[i]->begin()->second == synVal->getName()) {
			for(unsigned int j = 0; j < resultTuples[i]->size(); j++) {
				if(valSet.find(resultTuples[i]->at(j).second) == valSet.end()) {
					resultTuples[i]->erase(resultTuples[i]->begin() + j);
					j--;
				}
			}

			if(resultTuples[i]->size() == 1) {
				hasResult = false;
				return;
			}

			if(resultTuples[i]->size() < initSize) {
				set<string> leftVal;

				for(unsigned int j = 1; j < resultTuples[i]->size(); j++) {
					leftVal.insert(resultTuples[i]->at(j).first);
				}

				SynonymValues* leftSyn = getSynVal(resultTuples[i]->begin()->first);
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

void QueryEvaluator::formFinalResult(string s, unsigned int index) {
	ParamNode* node = resultSynonyms[index];
	SynonymValues* synVal = getSynVal(node->getParam());
	set<string> valSet = synVal->getValues();

	if(valSet.empty()) {
		vector<Node*> result = pkb->getNodes(resultSynonyms[index]->getType());

		if((node->getType() == call && node->getAttrType() == stringType) || node->getType() == procedure || node->getType() == variable || node->getType() == constant) {
			for(unsigned int i = 0; i < result.size(); i++) {
				valSet.insert(result[i]->getValue());
			}
		}
		else {
			for(unsigned int i = 0; i < result.size(); i++) {
				valSet.insert(result[i]->getLine());
			}
		}
	}
	else if(resultSynonyms[index]->getType() == call && resultSynonyms[index]->getAttrType() == stringType) {
		vector<Node*> result = pkb->getNodes(resultSynonyms[index]->getType());

		for(unsigned int i = 0; i < result.size(); i++) {
			string stmtLine = result[i]->getLine();

			if(valSet.find(stmtLine) != valSet.end()) {
				valSet.insert(result[i]->getValue());
				valSet.erase(stmtLine);
			}
		}
	}

	index++;

	for(set<string>::iterator i = valSet.begin(); i != valSet.end(); i++) {
		string singleResult = s;
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

SynonymValues* QueryEvaluator::getSynVal(string name) {

	for(unsigned int i = 0; i < synonymVec.size(); i++) {
		if(name == synonymVec[i]->getName()) {
			return synonymVec[i];
		}
	}

	return NULL;
}