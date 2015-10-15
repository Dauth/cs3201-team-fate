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
		for(vector<ParamNode*>::iterator i = resultSynonyms.begin(); i != resultSynonyms.end(); i++) {
			synonymVec.push_back(new SynonymValues((**i).getParam()));
		}

		sortQueryParts();

		if(!hasResult) {
			return;
		}
	}
	
	for(vector<QueryPart*>::iterator i = queryParts.begin(); i != queryParts.end(); i++) {
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
		for(vector<QueryPart*>::iterator i = queryParts.begin(); i != queryParts.end(); i++) {
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
					if(queryType == follows || queryType == parent || queryType == calls || queryType == nxt || queryType == with) {
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
				if(queryType == follows || queryType == parent || queryType == calls || queryType == nxt || queryType == with) {
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

				if(queryType == follows || queryType == parent || queryType == calls || queryType == nxt || queryType == with) {
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

				if(queryType == follows || queryType == parent || queryType == calls || queryType == nxt || queryType == with) {
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

bool QueryEvaluator::existsInSynVec(string name) {
	for(vector<SynonymValues*>::iterator i = synonymVec.begin(); i != synonymVec.end(); i++) {
		if(name == (**i).getName()) {
			return true;
		}
	}
	return false;
}

void QueryEvaluator::evalQueryWithNoResult() {
	for(vector<QueryPart*>::iterator i = queryWithNoResult.begin(); i != queryWithNoResult.end(); i++) {
		vector<pair<string, string>> result = getResult(*i);

		if(result.empty()) {
			hasResult = false;
			return;
		}
	}
}

void QueryEvaluator::evalQueryWithOneResult() {
	for(vector<QueryPart*>::iterator i = queryWithOneResult.begin(); i != queryWithOneResult.end(); i++) {
		vector<pair<string, string>> result = getResult(*i);

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
	for(vector<QueryPart*>::iterator i = queryWithTwoResults.begin(); i != queryWithTwoResults.end(); i++) {
		vector<pair<string, string>> result = getResult(*i);
		
		if(result.empty()) {
			hasResult = false;
			return;
		}

		updateSynVal((**i).getLeftParam(), (**i).getRightParam(), result);
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

	if(left->getType() == integer) {
		vector<Node*> nodes = pkb->getNodes(right->getType());

		for(vector<Node*>::iterator i = nodes.begin(); i != nodes.end(); i++) {
			if((**i).getLine() == left->getParam()) {
				pair<string, string> p;
				p.first = "";
				p.second = left->getParam();
				result.push_back(p);
				break;
			}
		}
	}
	else if(left->getType() == expression) {
		vector<Node*> nodes = pkb->getNodes(right->getType());

		for(vector<Node*>::iterator i = nodes.begin(); i != nodes.end(); i++) {
			string value;

			value = (**i).getValue();

			if(value == left->getParam()) {
				if(right->getType() == call) {
					value = (**i).getLine();
				}

				pair<string, string> p;
				p.first = "";
				p.second = value;
				result.push_back(p);
				break;
			}
		}
	}
	else if(right->getType() == integer) {
		vector<Node*> nodes = pkb->getNodes(left->getType());

		for(vector<Node*>::iterator i = nodes.begin(); i != nodes.end(); i++) {
			if((**i).getLine() == right->getParam()) {
				pair<string, string> p;
				p.first = right->getParam();
				p.second = "";
				result.push_back(p);
				break;
			}
		}
	}
	else if(right->getType() == expression) {
		vector<Node*> nodes = pkb->getNodes(left->getType());

		for(vector<Node*>::iterator i = nodes.begin(); i != nodes.end(); i++) {
			string value;

			value = (**i).getValue();

			if(value == right->getParam()) {
				if(left->getType() == call) {
					value = (**i).getLine();
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
			for(vector<Node*>::iterator i = leftNodes.begin(); i != leftNodes.end(); i++) {
				for(vector<Node*>::iterator j = rightNodes.begin(); j != rightNodes.end(); j++) {
					if((**i).getValue() == (**j).getValue()) {
						pair<string, string> p;

						if(left->getType() == call) {
							p.first = (**i).getLine();
						}
						else {
							p.first = (**i).getValue();
						}

						if(right->getType() == call) {
							p.second = (**i).getLine();
						}
						else {
							p.second = (**i).getValue();
						}

						result.push_back(p);
					}
				}
			}
		}
		else {
			for(vector<Node*>::iterator i = leftNodes.begin(); i != leftNodes.end(); i++) {
				for(vector<Node*>::iterator j = rightNodes.begin(); j != rightNodes.end(); j++) {
					if((**i).getLine() == (**j).getLine()) {
						pair<string, string> p;
						p.first = (**i).getLine();
						p.second = (**i).getLine();
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
/*		case nxt		:
			return pkb->getNext(left, right);
		case nxtStar	:
			return pkb->getNextStar(left, right);
		case affects	:
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
/*		case nxt		:
			return pkb->getNext(left, right);
		case nxtStar	:
			return pkb->getNextStar(left, right);
		case affects	:
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
/*		case nxt		:
			return pkb->getNext(left, right);
		case nxtStar	:
			return pkb->getNextStar(left, right);
		case affects	:
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
/*		case nxt		:
			return pkb->getNext(left, right);
		case nxtStar	:
			return pkb->getNextStar(left, right);
		case affects	:
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
			for(vector<pair<string, string>>::iterator i = vec.begin(); i != vec.end(); i++) {
				if(leftVal.find(i->first) == leftVal.end()) {
					vec.erase(i);
					i--;
				}
			}
		}

		if(!rightVal.empty()) {
			for(vector<pair<string, string>>::iterator i = vec.begin(); i != vec.end(); i++) {
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

		set<string> newLeftVal;
		set<string> newRightVal;

		for(vector<pair<string, string>>::iterator i = vec.begin(); i != vec.end(); i++) {
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
		
		vector<pair<string, string>> tuples;
		pair<string, string> p;
		p.first = leftSyn->getName();
		p.second = rightSyn->getName();
		tuples.push_back(p);

		for(vector<pair<string, string>>::iterator i = vec.begin(); i != vec.end(); i++) {
			tuples.push_back(*i);
		}

		resultTuples.push_back(&tuples);
	}
	else if(lNode != NULL) {
		SynonymValues* leftSyn = getSynVal(lNode->getParam());
		set<string> leftVal = leftSyn->getValues();

		if(!leftVal.empty()) {
			for(vector<pair<string, string>>::iterator i = vec.begin(); i != vec.end(); i++) {
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

		set<string> newLeftVal;

		for(vector<pair<string, string>>::iterator i = vec.begin(); i != vec.end(); i++) {
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
		set<string> rightVal = rightSyn->getValues();

		if(!rightVal.empty()) {
			for(vector<pair<string, string>>::iterator i = vec.begin(); i != vec.end(); i++) {
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

		set<string> newRightVal;

		for(vector<pair<string, string>>::iterator i = vec.begin(); i != vec.end(); i++) {
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
	set<string> valSet = synVal->getValues();

	for(vector<vector<pair<string, string>>*>::iterator i = resultTuples.begin(); i != resultTuples.end(); i++) {
		unsigned int initSize = (**i).size();

		if((**i).begin()->first == synVal->getName()) {
			for(vector<pair<string, string>>::iterator j = (**i).begin() + 1; j != (**i).end(); j++) {
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
				set<string> rightVal;

				for(vector<pair<string, string>>::iterator j = (**i).begin() + 1; j != (**i).end(); j++) {
					rightVal.insert(j->second);
				}

				SynonymValues* rightSyn = getSynVal((**i).begin()->second);
				rightSyn->setValues(rightVal);
				updateRelatedSynVal(rightSyn);
			}
		}
		else if((**i).begin()->second == synVal->getName()) {
			for(vector<pair<string, string>>::iterator j = (**i).begin() + 1; j != (**i).end(); j++) {
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
				set<string> leftVal;

				for(vector<pair<string, string>>::iterator j = (**i).begin() + 1; j != (**i).end(); j++) {
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

void QueryEvaluator::formFinalResult(string s, unsigned int index) {
	ParamNode* node = resultSynonyms[index];
	SynonymValues* synVal = getSynVal(node->getParam());
	set<string> valSet = synVal->getValues();
	index++;

	if(valSet.empty()) {
		vector<Node*> result = pkb->getNodes(resultSynonyms[index]->getType());

		for(vector<Node*>::iterator i = result.begin(); i != result.end(); i++) {
			if((node->getType() == call && node->getAttrType() == stringType) || node->getType() == procedure || node->getType() == variable || node->getType() == constant) {
				for(vector<Node*>::iterator i = result.begin(); i != result.end(); i++) {
					valSet.insert((**i).getValue());
				}
			}
			else {
				for(vector<Node*>::iterator i = result.begin(); i != result.end(); i++) {
					valSet.insert((**i).getLine());
				}
			}
		}
	}
	else if(resultSynonyms[index]->getType() == call && resultSynonyms[index]->getAttrType() == stringType) {
		vector<Node*> result = pkb->getNodes(resultSynonyms[index]->getType());

		for(vector<Node*>::iterator i = result.begin(); i != result.end(); i++) {
			string stmtLine = (**i).getLine();

			if(valSet.find(stmtLine) != valSet.end()) {
				valSet.insert((**i).getValue());
				valSet.erase(stmtLine);
			}
		}
	}

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

	for(vector<SynonymValues*>::iterator i = synonymVec.begin(); i != synonymVec.end(); i++) {
		if(name == (**i).getName()) {
			return *i;
		}
	}

	return NULL;
}