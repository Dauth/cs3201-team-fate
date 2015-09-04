#include "ParentFunctions.h";
#include "ParentsTable.h"
#include "PKB.h"
#include <vector>;

ParentsTable pt;
ParentFunctions::ParentFunctions(){
};

std::vector<Node*> ParentFunctions::getChildrenStar(int statementLine){
	std::vector<Node*> childList = PKB::getChildren(statementLine);
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
	std::vector<Node*> childList = PKB::getChildren(type);
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
	parentList.push_back(PKB::getParent(statementLine));
	std::vector<Node*> tempList = parentList;
	for (unsigned int i=0; i<tempList.size(); i++){
		std::vector<Node*> tempList2 = ParentFunctions::getParentsStar(tempList.at(i)->getLine());
		parentList.insert(parentList.end(),tempList2.begin(),tempList2.end());
	}
	return parentList;
}

std::vector<Node*> ParentFunctions::getParentsStar(synt_type type){
	std::vector<Node*> parentList = PKB::getParent(type);
	std::vector<Node*> tempList = parentList;
	for (unsigned int i=0; i<tempList.size(); i++){
		std::vector<Node*> tempList2 = ParentFunctions::getParentsStar(tempList.at(i)->getType());
		parentList.insert(parentList.end(),tempList2.begin(),tempList2.end());
	}
	return parentList;
	
}