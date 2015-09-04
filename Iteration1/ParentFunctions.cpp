#include "ParentFunctions.h"
#include "PKB.h"
#include <vector>

ParentFunctions::ParentFunctions(PKB* i){pkb = i;}

std::vector<Node*> ParentFunctions::getChildrenStar(int statementLine){
	std::vector<Node*> childList;
	if (pkb->getChildren(statementLine).empty()){
		return childList;
	}
	childList = pkb->getChildren(statementLine);
	std::vector<Node*> tempList = childList;
	for (unsigned int i=0; i<tempList.size(); i++){
		if(tempList.at(i)->getType() == whileLoop){
			std::vector<Node*> tempList2 = ParentFunctions::getChildrenStar(tempList.at(i)->getLine());
			childList.insert(childList.end(),tempList2.begin(),tempList2.end());
		}
	}
	return childList;
	
}

std::vector<Node*> ParentFunctions::getChildrenStar(synt_type type){
	std::vector<Node*> childList;
	if (pkb->getChildren(type).empty()){
		return childList;
	}
	childList = pkb->getChildren(type);
	std::vector<Node*> tempList = childList;
	for (unsigned int i=0; i<tempList.size(); i++){
		if(tempList.at(i) -> getType() == whileLoop){
			std::vector<Node*> tempList2 = ParentFunctions::getChildrenStar(tempList.at(i)->getType());
			childList.insert(childList.end(),tempList2.begin(),tempList2.end());
		}
	}
	return childList;
	
}

std::vector<Node*> ParentFunctions::getParentsStar(int statementLine){
	std::vector<Node*> parentList;
	if (pkb->getParent(statementLine).empty()){
		return parentList;
	}
	parentList = pkb->getParent(statementLine);
	std::vector<Node*> tempList = parentList;
	for (unsigned int i=0; i<tempList.size(); i++){
		std::vector<Node*> tempList2 = ParentFunctions::getParentsStar(tempList.at(i)->getLine());
		parentList.insert(parentList.end(),tempList2.begin(),tempList2.end());
	}
	return parentList;
}

std::vector<Node*> ParentFunctions::getParentsStar(synt_type type){
	std::vector<Node*> parentList;
	if (pkb->getParent(type).empty()){
		return parentList;
	}
	parentList = pkb->getParent(type);
	std::vector<Node*> tempList = parentList;
	for (unsigned int i=0; i<tempList.size(); i++){
		std::vector<Node*> tempList2 = ParentFunctions::getParentsStar(tempList.at(i)->getType());
		parentList.insert(parentList.end(),tempList2.begin(),tempList2.end());
	}
	return parentList;
	
}