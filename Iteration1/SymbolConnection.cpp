#include "stdafx.h"
#include "SymbolConnection.h"

SymbolConnection::SymbolConnection () {
	
}

void SymbolConnection::addPattern(PatternNode* p) {
	if(pVec.empty()) {
		pVec.push_back(p);
	} else {
		bool push = true;
		for(std::vector<PatternNode*>::iterator i = pVec.begin(); i < pVec.end(); i++) {
			if(*i = p) {
				push = false;
			}
		}
		if(push) {
			pVec.push_back(p);
		}
	}
}

void SymbolConnection::addQuery(QueryNode* q) {
	if(qVec.empty()) {
		qVec.push_back(q);
	} else {
		bool push = true;
		for(std::vector<QueryNode*>::iterator i = qVec.begin(); i < qVec.end(); i++) {
			if(*i = q) {
				push = false;
			}
		}
		if(push) {
			qVec.push_back(q);
		}
	}
}

std::vector<PatternNode*> SymbolConnection::getPatterns(std::string sym) {
	std::vector<PatternNode*> patterns;
	for(std::vector<PatternNode*>::iterator i = pVec.begin(); i < pVec.end(); i++) {
		if((**i).getLeftParam()->getParam() == sym || (**i).getMiddleParam()->getParam() == sym) {
			patterns.push_back(*i);
		}
	}
	return patterns;
}

std::vector<QueryNode*> SymbolConnection::getQueries(std::string sym) {
	std::vector<QueryNode*> queries;
	for(std::vector<QueryNode*>::iterator i = qVec.begin(); i < qVec.end(); i++) {
		if((**i).getLeftParam()->getParam() == sym || (**i).getRightParam()->getParam() == sym) {
			queries.push_back(*i);
		}
	}
	return queries;
}