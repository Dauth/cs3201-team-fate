#include "stdafx.h"
#include "FollowsFunctions.h"
#include "PKB.h"
#include <vector>

FollowsFunctions::FollowsFunctions(PKB* p){
	pkb = p;
}

std::vector<Node*> FollowsFunctions::getFollowingStar(int statementLine){
	std::vector<Node*> followList;
	if (pkb->getFollowing(statementLine).empty()){
		return 	followList;
	}
	followList = pkb->getFollowing(statementLine);
	std::vector<Node*> tempList = followList;
	for (unsigned int i = 0 ; i<tempList.size();i++){
		std::vector<Node*> tempList2 = FollowsFunctions::getFollowingStar(tempList.at(i)->getLine());
		followList.insert(followList.end(),tempList2.begin(),tempList2.end());
	}

	return followList;
}

std::vector<Node*> FollowsFunctions::getFollowingStar(synt_type type){
	std::vector<Node*> followList;
	if (pkb->getFollowing(type).empty()){
		return 	followList;
	}
	followList = pkb->getFollowing(type);
	std::vector<Node*> tempList = followList;
	for (unsigned int i = 0 ; i<tempList.size();i++){
		std::vector<Node*> tempList2 = FollowsFunctions::getFollowingStar(tempList.at(i)->getType());
		followList.insert(followList.end(),tempList2.begin(),tempList2.end());
	}

	return followList;
}

std::vector<Node*> FollowsFunctions::getFollowedByStar(int statementLine){
	std::vector<Node*> followedBy;
	if (pkb->getFollowedBy(statementLine).empty()){
		return followedBy;
	}
	followedBy = pkb->getFollowedBy(statementList);
	std::vector<Node*> tempList = followedBy;
	for (unsigned int i = 0 ; i<tempList.size(); i++){
		std::vector<Node*> tempList2 = FollowsFunctions::getFollowedByStar(tempList.at(i)->getLine());
		followedBy.insert(followedBy.end(),tempList2.begin(),tempList2.end());
	}

	return followedBy;
}

std::vector<Node*> FollowsFunctions::getFollowedByStar(synt_type type){
	std::vector<Node*> followedBy;
	if (pkb->getFollowedBy(type).empty()){
		return followedBy;
	}
	followedBy = pkb->getFollowedBy(type);
	std::vector<Node*> tempList = followedBy;
	for (unsigned int i = 0 ; i<tempList.size(); i++){
		std::vector<Node*> tempList2 = FollowsFunctions::getFollowedByStar(tempList.at(i)->getType());
		followedBy.insert(followedBy.end(),tempList2.begin(),tempList2.end());
	}

	return followedBy;
}