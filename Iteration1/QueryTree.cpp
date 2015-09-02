#include "QueryTree.h"

QueryNode* firstQuery;
PatternNode* firstPattern;
QueryNode* currQuery;
PatternNode* currPattern;
QueryNode* lastAddedQuery;
PatternNode* lastAddedPattern;

QueryTree::QueryTree () {
	firstQuery = NULL;
	firstPattern = NULL;
	lastAddedQuery = NULL;
	lastAddedPattern = NULL;
}

QueryNode* QueryTree::getQuery() {
	if(currQuery == NULL) {
		currQuery = firstQuery;
	} else {
		currQuery = currQuery->getNext();
	}
	return currQuery;
}

PatternNode* QueryTree::getPattern() {
	if(currPattern == NULL) {
		currPattern = firstPattern;
	} else {
		currPattern = currPattern->getNext();
	}
	return currPattern;
}

void QueryTree::addQuery(QueryNode* query) {
	if(firstQuery == NULL) {
		firstQuery = query;
		lastAddedQuery = firstQuery;
	} else {
		lastAddedQuery->setNext(query);
		lastAddedQuery = lastAddedQuery->getNext();
	}
}

void QueryTree::addPattern(PatternNode* pattern) {
	if(firstPattern == NULL) {
		firstPattern = pattern;
		lastAddedPattern = firstPattern;
	} else {
		lastAddedPattern->setNext(pattern);
		lastAddedPattern = lastAddedPattern->getNext();
	}
}