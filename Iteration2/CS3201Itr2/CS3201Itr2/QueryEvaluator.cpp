#include "stdafx.h"
#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator(PKB* p) {
	pkb = p;
	callNodes = pkb->getNodes(call);
}

//method called to evaluate a PQL query
list<string> QueryEvaluator::evaluate(vector<ParamNode*> rVec, vector<QueryPart*> qVec) {
	resultSynonyms = rVec;
	queryParts = qVec;
	hasResult = true;
	timedOut = false;
	
	queryWithNoResult.clear();
	queryWithOneResult.clear();
	queryWithTwoResults.clear();
	synonymVec.clear();
	resultTuples.clear();
	finalResult.clear();
	optimise();

	if(hasResult && !queryWithNoResult.empty()) {
			evalQueryWithNoResult();
	}

	if(hasResult && !queryWithOneResult.empty()) {
			evalQueryWithOneResult();
	}

	if(hasResult && !queryWithTwoResults.empty()) {
			evalQueryWithTwoResults();
	}
	
	if(!timedOut) {
		evalFinalResult();
	}

	return finalResult;
}

//optimises evaluation process by filtering, sorting and ordering QueryParts
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
	
	//QueryParts not sorted into groups will be put into queryWithNoResult vector
	for(unsigned int i = 0; i < queryParts.size(); i++) {
		QueryType queryType = queryParts[i]->getType();
		ParamNode* left = queryParts[i]->getLeftParam();
		ParamNode* right = queryParts[i]->getRightParam();

		//filter QueryParts such as "Next(s,s)" that guarantees empty result
		if((queryType == nxt || queryType == follows || queryType == followsStar || queryType == parent || queryType == parentStar) && left->getParam() == right->getParam()) {
			hasResult = false;
			return;
		}

		//filter QueryParts such as "Affects(w,a)" that guarantees empty result
		if((queryType == affects || queryType == affectsStar) && (left->getType() == whileLoop || left->getType() == ifelse || left->getType() == call || right->getType() == whileLoop || right->getType() == ifelse || right->getType() == call)) {
			hasResult = false;
			return;
		}

		//filter QueryParts such as "a.stmt#=a.stmt#" that guarantees true and QueryParts such as "1=2" that guarantees false
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

		//ensure QueryParts with 2 synonyms in queryWithNoResult vector to be at the back
		if(left->getType() != integer && left->getType() != expression && right->getType() != integer && right->getType() != expression) {
			queryWithNoResult.push_back(queryParts[i]);
		}
		else {
			queryWithNoResult.insert(queryWithNoResult.begin(), queryParts[i]);
		}
	}

	if(AbstractWrapper::GlobalStop) {
		queryWithNoResult.clear();
		queryWithOneResult.clear();
		queryWithTwoResults.clear();
		synonymVec.clear();
		resultTuples.clear();
		finalResult.clear();
		hasResult = false;
		timedOut = true;
		return;
	}

	//further order QueryParts in groups to improve optimisation
	orderQueryParts(&queryWithNoResult);
	orderQueryParts(&queryWithOneResult);
	orderQueryParts(&queryWithTwoResults);
}

//sorts all the QueryParts into 3 groups depending on the number of useful synonyms they have
//useful synonyms are synonyms in the query that contributes to the final result
void QueryEvaluator::sortQueryParts() {
	int initSize = 1;
	int currSize = 0;

	while(currSize < initSize) {

		if(AbstractWrapper::GlobalStop) {
			queryWithNoResult.clear();
			queryWithOneResult.clear();
			queryWithTwoResults.clear();
			synonymVec.clear();
			resultTuples.clear();
			finalResult.clear();
			hasResult = false;
			timedOut = true;
			return;
		}

		initSize = queryParts.size();

		for(unsigned int i = 0; i < queryParts.size(); i++) {
			QueryType queryType = queryParts[i]->getType();
			ParamNode* left = queryParts[i]->getLeftParam();
			ParamNode* right = queryParts[i]->getRightParam();

			//filter QueryParts such as "Next(s,s)" that guarantees empty result
			if((queryType == nxt || queryType == follows || queryType == followsStar || queryType == parent || queryType == parentStar) && left->getParam() == right->getParam()) {
				hasResult = false;
				return;
			}

			//filter QueryParts such as "Affects(w,a)" that guarantees empty result
			if((queryType == affects || queryType == affectsStar) && (left->getType() == whileLoop || left->getType() == ifelse || left->getType() == call || right->getType() == whileLoop || right->getType() == ifelse || right->getType() == call)) {
				hasResult = false;
				return;
			}

			//filter QueryParts such as "a.stmt#=a.stmt#" that guarantees true and QueryParts such as "1=2" that guarantees false
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

			if(left->getType() == integer || left->getType() == expression) {
				if(right->getType() == integer || right->getType() == expression) {
					queryWithNoResult.push_back(queryParts[i]);

					queryParts.erase(queryParts.begin() + i);
					i--;
				}
				else if(existsInSynVec(right->getParam())) {
					//QueryParts that takes shorter time to evaluate is inserted into the front of the group
					if(queryType == follows || queryType == parent || queryType == calls || queryType == with) {
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
				//QueryParts that takes less time to evaluate is inserted into the front of the group
				if(queryType == follows || queryType == parent || queryType == calls || queryType == with) {
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

				//QueryParts that takes less time to evaluate is inserted into the front of the group
				if(queryType == follows || queryType == parent || queryType == calls || queryType == with) {
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

				//QueryParts that takes less time to evaluate is inserted into the front of the group
				if(queryType == follows || queryType == parent || queryType == calls || queryType == with) {
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

//checks if a synonym exists in synonymVec
bool QueryEvaluator::existsInSynVec(string name) {
	for(unsigned int i = 0; i < synonymVec.size(); i++) {
		if(name == synonymVec[i]->getName()) {
			return true;
		}
	}
	return false;
}

//orders QueryParts in a group by their types in the order (front)others->next->next*->affects->affects*(end)
void QueryEvaluator::orderQueryParts(vector<QueryPart*>* qVec) {
	for(int i = 10; i < 14; i++) {
		for(unsigned int j = 0; j < qVec->size(); j++) {
			if(qVec->at(j)->getType() == i) {
				qVec->push_back(qVec->at(j));
				qVec->erase(qVec->begin() + j);
			}
		}
	}
}

void QueryEvaluator::evalQueryWithNoResult() {
	for(unsigned int i = 0; i < queryWithNoResult.size(); i++) {
		vector<pair<string, string>> result = getResult(queryWithNoResult[i]);

		if(AbstractWrapper::GlobalStop) {
			queryWithNoResult.clear();
			queryWithOneResult.clear();
			queryWithTwoResults.clear();
			synonymVec.clear();
			resultTuples.clear();
			finalResult.clear();
			hasResult = false;
			timedOut = true;
			return;
		}

		if(result.empty()) {
			hasResult = false;
			return;
		}
	}
}

void QueryEvaluator::evalQueryWithOneResult() {
	for(unsigned int i = 0; i < queryWithOneResult.size(); i++) {
		vector<pair<string, string>> result = getResult(queryWithOneResult[i]);

		if(AbstractWrapper::GlobalStop) {
			queryWithNoResult.clear();
			queryWithOneResult.clear();
			queryWithTwoResults.clear();
			synonymVec.clear();
			resultTuples.clear();
			finalResult.clear();
			hasResult = false;
			timedOut = true;
			return;
		}

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

		if(AbstractWrapper::GlobalStop) {
			queryWithNoResult.clear();
			queryWithOneResult.clear();
			queryWithTwoResults.clear();
			synonymVec.clear();
			resultTuples.clear();
			finalResult.clear();
			hasResult = false;
			timedOut = true;
			return;
		}
		
		if(result.empty()) {
			hasResult = false;
			return;
		}
		
		updateSynVal(queryWithTwoResults[i]->getLeftParam(), queryWithTwoResults[i]->getRightParam(), result);
	}
}

//gets result for a QueryPart
vector<pair<string, string>> QueryEvaluator::getResult(QueryPart* qp) {
	if(qp->getType() == with) {
		return evalWithQuery(qp);
	}

	ParamNode* left = qp->getLeftParam();
	ParamNode* right = qp->getRightParam();

	if(left->getType() == integer || left->getType() == expression) {
		if(left->getParam() == "_") {
			SyntType leftType = getSyntType(qp->getType());
			if(right->getType() == integer || right->getType() == expression) {
				if(right->getParam() == "_") {
					SyntType rightType = getSyntType(qp->getType());
					return getResultFromPKB(qp->getType(), leftType, rightType);
				}
				else {
					return getResultFromPKB(qp->getType(), leftType, right->getParam());
				}
			}
			else {
				return getResultFromPKB(qp->getType(), leftType, right->getType());
			}
		}
		else {
			if(right->getType() == integer || right->getType() == expression) {
				if(right->getParam() == "_") {
					SyntType rightType = getSyntType(qp->getType());
					return getResultFromPKB(qp->getType(), left->getParam(), rightType);
				}
				else {
					return getResultFromPKB(qp->getType(), left->getParam(), right->getParam());
				}
			}
			else {
				return getResultFromPKB(qp->getType(), left->getParam(), right->getType());
			}
		}
	}
	else if(right->getType() == integer || right->getType() == expression) {
		if(qp->getType() == pattern) {
			return pkb->searchWithPattern(left->getType(), right->getParam(), qp->getLastParam()->getParam());
		}
		else {
			if(right->getParam() == "_") {
				SyntType rightType = getSyntType(qp->getType());
				return getResultFromPKB(qp->getType(), left->getType(), rightType);
			}
			else {
				return getResultFromPKB(qp->getType(), left->getType(), right->getParam());
			}
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

//gets the appropriate SyntType that represents "_" in a QueryPart
SyntType QueryEvaluator::getSyntType(QueryType qType) {
	if(qType == calls || qType == callsStar) {
		return procedure;
	}
	else if(qType == modifies || qType == uses) {
		return variable;
	}
	else {
		return statement;
	}
}

//evaluates QueryParts of type "with" in QueryEvaluator with the help of PKB
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
		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);
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
		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);
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
		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);
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
		case affects	:
			return pkb->getAffects(left, right);
		case affectsStar:
			return pkb->getAffectsStar(left, right);
		default			:
			vector<pair<string, string>> empty;
			return empty;
	}
}

//filters results of evaluated QueryParts and updates values of synonyms
void QueryEvaluator::updateSynVal(ParamNode* lNode, ParamNode* rNode, vector<pair<string, string>> vec) {
	if(lNode != NULL && rNode != NULL && lNode->getParam() != rNode->getParam()) {
		updateTwoSynVal(lNode, rNode, vec);
	}
	else if(lNode != NULL) {
		updateLeftSynVal(lNode, rNode, vec);
	}
	else {
		updateRightSynVal(lNode, rNode, vec);
	}
}

void QueryEvaluator::updateTwoSynVal(ParamNode* lNode, ParamNode* rNode, vector<pair<string, string>> vec) {
	SynonymValues* leftSyn = getSynVal(lNode->getParam());
	SynonymValues* rightSyn = getSynVal(rNode->getParam());
	set<string> leftVal = leftSyn->getValues();
	set<string> rightVal = rightSyn->getValues();

	//filter pairs of results with existing pairs of values that belong to the same synonyms
	for(unsigned int i = 0; i < resultTuples.size(); i++) {
		if(resultTuples[i][0].first == lNode->getParam() && resultTuples[i][0].second == rNode->getParam()) {
			for(unsigned int j = 0; j < vec.size(); j++) {
				bool pairExists = false;

				for(unsigned int k = 1; k < resultTuples[i].size(); k++) {
					if(vec[j].first == resultTuples[i][k].first && vec[j].second == resultTuples[i][k].second) {
						pairExists = true;
						break;
					}
				}

				if(!pairExists) {
					vec.erase(vec.begin() + j);
					j--;
				}
			}

			resultTuples.erase(resultTuples.begin() + i);
			break;
		}
		else if(resultTuples[i][0].first == rNode->getParam() && resultTuples[i][0].second == lNode->getParam()) {
			for(unsigned int j = 0; j < vec.size(); j++) {
				bool pairExists = false;

				for(unsigned int k = 1; k < resultTuples[i].size(); k++) {
					if(vec[j].first == resultTuples[i][k].second && vec[j].second == resultTuples[i][k].first) {
						pairExists = true;
						break;
					}
				}

				if(!pairExists) {
					vec.erase(vec.begin() + j);
					j--;
				}
			}

			resultTuples.erase(resultTuples.begin() + i);
			break;
		}
	}

	if(vec.empty()) {
		hasResult = false;
		return;
	}

	//filter left of results with existing values of the same synonym
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

	//filter right of results with existing values of the same synonym
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

	//update values of left synonym
	if(leftVal.empty()) {
		leftSyn->setValues(newLeftVal);
	}
	else if(newLeftVal.size() < leftVal.size()) {
		leftSyn->setValues(newLeftVal);
		updateRelatedSynVal(leftSyn);
	}

	//update values of right synonym
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

	resultTuples.push_back(tuples);
}

void QueryEvaluator::updateLeftSynVal(ParamNode* lNode, ParamNode* rNode, vector<pair<string, string>> vec) {
	//if both sides of results are values of the same synonym
	//filter pairs that have different values on both sides
	if(rNode != NULL) {
		for(unsigned int i = 0; i < vec.size(); i++) {
			if(vec[i].first != vec[i].second) {
				vec.erase(vec.begin() + i);
				i--;
			}
		}

		if(vec.empty()) {
			hasResult = false;
			return;
		}
	}

	SynonymValues* leftSyn = getSynVal(lNode->getParam());
	set<string> leftVal = leftSyn->getValues();

	//filter left of results with existing values of the same synonym
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

	//update values of left synonym
	if(leftVal.empty()) {
		leftSyn->setValues(newLeftVal);
	}
	else if(newLeftVal.size() < leftVal.size()) {
		leftSyn->setValues(newLeftVal);
		updateRelatedSynVal(leftSyn);
	}
}

void QueryEvaluator::updateRightSynVal(ParamNode* lNode, ParamNode* rNode, vector<pair<string, string>> vec) {
	SynonymValues* rightSyn = getSynVal(rNode->getParam());
	set<string> rightVal = rightSyn->getValues();

	//filter left of results with existing values of the same synonym
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

	//update values of left synonym
	if(rightVal.empty()) {
		rightSyn->setValues(newRightVal);
	}
	else if(newRightVal.size() < rightVal.size()) {
		rightSyn->setValues(newRightVal);
		updateRelatedSynVal(rightSyn);
	}
}

//re-filters and updates values of related synonyms
void QueryEvaluator::updateRelatedSynVal(SynonymValues* synVal) {
	if(AbstractWrapper::GlobalStop) {
		queryWithNoResult.clear();
		queryWithOneResult.clear();
		queryWithTwoResults.clear();
		synonymVec.clear();
		resultTuples.clear();
		finalResult.clear();
		hasResult = false;
		timedOut = true;
		return;
	}

	set<string> valSet = synVal->getValues();

	for(unsigned int i = 0; i < resultTuples.size(); i++) {
		unsigned int initSize = resultTuples[i].size();

		if(resultTuples[i][0].first == synVal->getName()) {
			for(unsigned int j = 1; j < resultTuples[i].size(); j++) {
				if(valSet.find(resultTuples[i][j].first) == valSet.end()) {
					resultTuples[i].erase(resultTuples[i].begin() + j);
					j--;
				}
			}

			if(resultTuples[i].size() == 1) {
				hasResult = false;
				return;
			}

			if(resultTuples[i].size() < initSize) {
				set<string> rightVal;

				for(unsigned int j = 1; j < resultTuples[i].size(); j++) {
					rightVal.insert(resultTuples[i][j].second);
				}

				//update values of synonym
				SynonymValues* rightSyn = getSynVal(resultTuples[i][0].second);
				if(rightSyn->getValues().size() > rightVal.size()) {
					rightSyn->setValues(rightVal);
					updateRelatedSynVal(rightSyn);
				}
			}
		}
		else if(resultTuples[i][0].second == synVal->getName()) {
			for(unsigned int j = 1; j < resultTuples[i].size(); j++) {
				if(valSet.find(resultTuples[i][j].second) == valSet.end()) {
					resultTuples[i].erase(resultTuples[i].begin() + j);
					j--;
				}
			}

			if(resultTuples[i].size() == 1) {
				hasResult = false;
				return;
			}

			if(resultTuples[i].size() < initSize) {
				set<string> leftVal;

				for(unsigned int j = 1; j < resultTuples[i].size(); j++) {
					leftVal.insert(resultTuples[i][j].first);
				}

				//update values of synonym
				SynonymValues* leftSyn = getSynVal(resultTuples[i][0].first);
				if(leftSyn->getValues().size() > leftVal.size()) {
					leftSyn->setValues(leftVal);
					updateRelatedSynVal(leftSyn);
				}
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
		vector<vector<string>> empty;
		formFinalResult(empty);
	}
}

//forms the list of strings for the final result
void QueryEvaluator::formFinalResult(vector<vector<string>> parentRows) {
	vector<vector<string>> rows = formRows(parentRows);

	if(AbstractWrapper::GlobalStop) {
		queryWithNoResult.clear();
		queryWithOneResult.clear();
		queryWithTwoResults.clear();
		synonymVec.clear();
		resultTuples.clear();
		finalResult.clear();
		hasResult = false;
		timedOut = true;
		return;
	}
	
	if(!rows.empty()) {
		if(rows[0].size() < resultSynonyms.size()) {
			formFinalResult(rows);
		}
		else {
			formStringResult(rows);
		}
	}
}

//joins the values of the result synonyms
vector<vector<string>> QueryEvaluator::formRows(vector<vector<string>> parentRows) {
	vector<vector<string>> rows;

	if(parentRows.empty()) {
		ParamNode* node = resultSynonyms[0];
		SynonymValues* synVal = getSynVal(node->getParam());
		set<string> valSet = synVal->getValues();

		if(valSet.empty()) {
			vector<Node*> result = pkb->getNodes(node->getType());

			if(node->getType() == procedure || node->getType() == variable || node->getType() == constant) {
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

		for(set<string>::iterator i = valSet.begin(); i != valSet.end(); i++) {
			vector<string> currentRow;
			currentRow.push_back(*i);
			rows.push_back(currentRow);
		}
	}
	else {
		int curIndex = parentRows[0].size();
		bool tuplesExist = false;

		for(unsigned int h = 0; h < curIndex; h++) {
			for(unsigned int i = 0; i < resultTuples.size(); i++) {
				if(resultTuples[i][0].first == resultSynonyms[h]->getParam() && resultTuples[i][0].second == resultSynonyms[curIndex]->getParam()) {
					//if values in parentRow and values of current synonym has not been added to rows, add them to rows
					//else, compare corresponding 2 values in each "row" of rows with every pair values in current resultTuples
					//if the 2 values does not exist in current resultTuples, remove the "row"
					if(rows.empty()) {
						tuplesExist = true;

						for(unsigned int j = 0; j < parentRows.size(); j++) {
							for(unsigned int k = 1; k < resultTuples[i].size(); k++) {
								if(parentRows[j][h] == resultTuples[i][k].first) {
									vector<string> currentRow (parentRows[j]);
									currentRow.push_back(resultTuples[i][k].second);
									rows.push_back(currentRow);
								}
							}
						}
					}
					else {
						for(unsigned int j = 0; j < rows.size(); j++) {
							bool pairExists = false;

							for(unsigned int k = 1; k < resultTuples[i].size(); k++) {
								if(rows[j][h] == resultTuples[i][k].first && rows[j][curIndex] == resultTuples[i][k].second) {
									pairExists = true;
									break;
								}
							}

							if(!pairExists) {
								rows.erase(rows.begin() + j);
								j--;
							}
						}
					}

					break;
				}
				else if(resultTuples[i][0].first == resultSynonyms[curIndex]->getParam() && resultTuples[i][0].second == resultSynonyms[h]->getParam()) {
					//if values in parentRow and values of current synonym has not been added to rows, add them to rows
					//else, compare corresponding 2 values in each "row" of rows with every pair values in current resultTuples
					//if the 2 values does not exist in current resultTuples, remove the "row"
					if(rows.empty()) {
						tuplesExist = true;

						for(unsigned int j = 0; j < parentRows.size(); j++) {
							for(unsigned int k = 1; k < resultTuples[i].size(); k++) {
								if(parentRows[j][h] == resultTuples[i][k].second) {
									vector<string> currentRow (parentRows[j]);
									currentRow.push_back(resultTuples[i][k].first);
									rows.push_back(currentRow);
								}
							}
						}
					}
					else {
						for(unsigned int j = 0; j < rows.size(); j++) {
							bool pairExists = false;

							for(unsigned int k = 1; k < resultTuples[i].size(); k++) {
								if(rows[j][h] == resultTuples[i][k].second && rows[j][curIndex] == resultTuples[i][k].first) {
									pairExists = true;
									break;
								}
							}

							if(!pairExists) {
								rows.erase(rows.begin() + j);
								j--;
							}
						}
					}

					break;
				}
			}
		}

		if(!tuplesExist) {
			ParamNode* node = resultSynonyms[curIndex];
			SynonymValues* synVal = getSynVal(node->getParam());
			set<string> valSet = synVal->getValues();

			if(valSet.empty()) {
				vector<Node*> result = pkb->getNodes(node->getType());

				if(node->getType() == procedure || node->getType() == variable || node->getType() == constant || node->getType() == statementList) {
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

			for(unsigned int i = 0; i < parentRows.size(); i++) {
				for(set<string>::iterator j = valSet.begin(); j != valSet.end(); j++) {
					vector<string> currentRow (parentRows[i]);
					currentRow.push_back(*j);
					rows.push_back(currentRow);
				}
			}
		}
	}

	return rows;
}

//converts individual values to strings of final result
void QueryEvaluator::formStringResult(vector<vector<string>> rows) {
	//if final result requires call.procName, change values of call to call.procName values
	for(unsigned int i = 0; i < resultSynonyms.size(); i++) {
		if(resultSynonyms[i]->getType() == call && resultSynonyms[i]->getAttrType() == stringType) {
			for(unsigned int j = 0; j < rows.size(); j++) {
				for(unsigned int k = 0; k < callNodes.size(); k++) {
					if(rows[j][i] == callNodes[k]->getLine()) {
						rows[j].erase(rows[j].begin() + i);
						rows[j].insert(rows[j].begin() + i, callNodes[k]->getValue());
						break;
					}
				}
			}
		}
	}

	//convert individual values to strings
	for(unsigned int i = 0; i < rows.size(); i++) {
		string tuple;

		for(unsigned int j = 0; j < resultSynonyms.size(); j++) {
			tuple.append(rows[i][j]);

			if(j + 1 < resultSynonyms.size()) {
				tuple.append(" ");
			}
		}

		finalResult.push_back(tuple);
	}
}

//returns the set of values for a synonym
SynonymValues* QueryEvaluator::getSynVal(string name) {

	for(unsigned int i = 0; i < synonymVec.size(); i++) {
		if(name == synonymVec[i]->getName()) {
			return synonymVec[i];
		}
	}

	return NULL;
}