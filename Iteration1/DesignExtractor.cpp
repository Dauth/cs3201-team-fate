#include "stdafx.h"
#include "DesignExtractor.h"
#include "PKB.h"
#include <vector>

DesignExtractor::DesignExtractor(PKB* p){pkb = p;}

//Functions for Parent*

std::vector<Node*> DesignExtractor::getChildrenStar(int statementLine){
	std::vector<Node*> childList;
	if (pkb->getChildren(statementLine).empty()){
		return childList;
	}
	childList = pkb->getChildren(statementLine);
	std::vector<Node*> tempList = childList;
	for (unsigned int i=0; i<tempList.size(); i++){
		if(tempList.at(i)->getType() == whileLoop){
			std::vector<Node*> tempList2 = DesignExtractor::getChildrenStar(tempList.at(i)->getLine());
			childList.insert(childList.end(),tempList2.begin(),tempList2.end());
		}
	}
	return childList;
	
}

std::vector<Node*> DesignExtractor::getChildrenStar(synt_type type){
	std::vector<Node*> childList;
	if (pkb->getChildren(type).empty()){
		return childList;
	}
	childList = pkb->getChildren(type);
	std::vector<Node*> tempList = childList;
	for (unsigned int i=0; i<tempList.size(); i++){
		if(tempList.at(i) -> getType() == whileLoop){
			std::vector<Node*> tempList2 = DesignExtractor::getChildrenStar(tempList.at(i)->getType());
			childList.insert(childList.end(),tempList2.begin(),tempList2.end());
		}
	}
	return childList;
	
}

std::vector<Node*> DesignExtractor::getParentsStar(int statementLine){
	std::vector<Node*> parentList;
	if (pkb->getParent(statementLine).empty()){
		return parentList;
	}
	parentList = pkb->getParent(statementLine);
	std::vector<Node*> tempList = parentList;
	for (unsigned int i=0; i<tempList.size(); i++){
		std::vector<Node*> tempList2 = DesignExtractor::getParentsStar(tempList.at(i)->getLine());
		parentList.insert(parentList.end(),tempList2.begin(),tempList2.end());
	}
	return parentList;
}

std::vector<Node*> DesignExtractor::getParentsStar(synt_type type){
	std::vector<Node*> parentList;
	if (pkb->getParent(type).empty()){
		return parentList;
	}
	parentList = pkb->getParent(type);
	std::vector<Node*> tempList = parentList;
	for (unsigned int i=0; i<tempList.size(); i++){
		std::vector<Node*> tempList2 = DesignExtractor::getParentsStar(tempList.at(i)->getType());
		parentList.insert(parentList.end(),tempList2.begin(),tempList2.end());
	}
	return parentList;
	
}

//Functions for Follow*

std::vector<Node*> DesignExtractor::getFollowingStar(int statementLine){
	std::vector<Node*> followList;
	if (pkb->getFollowing(statementLine).empty()){
		return 	followList;
	}
	followList = pkb->getFollowing(statementLine);
	std::vector<Node*> tempList = followList;
	for (unsigned int i = 0 ; i<tempList.size();i++){
		std::vector<Node*> tempList2 = DesignExtractor::getFollowingStar(tempList.at(i)->getLine());
		followList.insert(followList.end(),tempList2.begin(),tempList2.end());
	}

	return followList;
}

std::vector<Node*> DesignExtractor::getFollowingStar(synt_type type){
	std::vector<Node*> followList;
	if (pkb->getFollowing(type).empty()){
		return 	followList;
	}
	followList = pkb->getFollowing(type);
	std::vector<Node*> tempList = followList;
	for (unsigned int i = 0 ; i<tempList.size();i++){
		std::vector<Node*> tempList2 = DesignExtractor::getFollowingStar(tempList.at(i)->getType());
		followList.insert(followList.end(),tempList2.begin(),tempList2.end());
	}

	return followList;
}

std::vector<Node*> DesignExtractor::getFollowedByStar(int statementLine){
	std::vector<Node*> followedBy;
	if (pkb->getFollowedBy(statementLine).empty()){
		return followedBy;
	}
	followedBy = pkb->getFollowedBy(statementList);
	std::vector<Node*> tempList = followedBy;
	for (unsigned int i = 0 ; i<tempList.size(); i++){
		std::vector<Node*> tempList2 = DesignExtractor::getFollowedByStar(tempList.at(i)->getLine());
		followedBy.insert(followedBy.end(),tempList2.begin(),tempList2.end());
	}

	return followedBy;
}

std::vector<Node*> DesignExtractor::getFollowedByStar(synt_type type){
	std::vector<Node*> followedBy;
	if (pkb->getFollowedBy(type).empty()){
		return followedBy;
	}
	followedBy = pkb->getFollowedBy(type);
	std::vector<Node*> tempList = followedBy;
	for (unsigned int i = 0 ; i<tempList.size(); i++){
		std::vector<Node*> tempList2 = DesignExtractor::getFollowedByStar(tempList.at(i)->getType());
		followedBy.insert(followedBy.end(),tempList2.begin(),tempList2.end());
	}

	return followedBy;
}