#include "stdafx.h"
#include "DesignExtractor.h"
#include "ExpressionTree.h"
#include "PKB.h"
#include <vector>

DesignExtractor::DesignExtractor(PKB* p){
	pkb = p;
	ExpressionTree et;
}

//Functions for Parent*
std::vector<Node*> DesignExtractor::getChildrenStar(int statementLine){
	std::vector<Node*> childList;
	if (pkb->getChildren(statementLine).empty()){
		return childList;
	}
	childList = pkb->getChildren(statementLine);
	std::vector<Node*> tempList = childList;
	for (unsigned int i=0 ; i<tempList.size() ; i++){
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
	for (unsigned int i=0 ; i<tempList.size() ; i++){
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
	for (unsigned int i=0 ; i<tempList.size() ; i++){
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
	for (unsigned int i=0 ; i<tempList.size() ; i++){
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
	for (unsigned int i = 0 ; i<tempList.size() ; i++){
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
	for (unsigned int i = 0 ; i<tempList.size() ; i++){
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
	for (unsigned int i = 0 ; i<tempList.size() ; i++){
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
	for (unsigned int i = 0 ; i<tempList.size() ; i++){
		std::vector<Node*> tempList2 = DesignExtractor::getFollowedByStar(tempList.at(i)->getType());
		followedBy.insert(followedBy.end(),tempList2.begin(),tempList2.end());
	}

	return followedBy;
}

//Functions for pattern matching

std::vector<Node*> DesignExtractor::searchWithPattern(synt_type type,std::string left,std::string right){
	std::vector<Node*> result;
	//if the pattern is for a while loop
	if (type == whileLoop){
		std::vector<Node*> whileList = pkb->getStatement(whileLoop);
		if (left == "_"){
			result = whileList;
		}
		else{
			for (unsigned int i = 0 ; i < whileList.size() ; i++){
				if (whileList.at(i)->getLeftChild()->getVariable()->getName() == left){
					result.push_back(whileList.at(i));
				}
			}
		}
	}

	//elseif type = "ifstat"
	//if the pattern is for an assignment
	else{
		//if the expression was "_"
		if (right == "_"){
			if (left == "_"){
				result = pkb->getStatement(assignment);
				return result;
			}
			else{
				std::vector<Node*> assignList  = pkb->getStatement(assignment);
				for(unsigned int i = 0 ; i < assignList.size() ; i++){
					if(assignList.at(i)->getLeftChild()->getVariable()->getName() == left){
						result.push_back(assignList.at(i));
					}
				}

				return result;
			}
		}

		//if the expression was not "_"
		Node* tNode;
		std::string firstChar = right.substr(0,0);

		if (firstChar != "_"){
			tNode = et.exptreeSetupSON(et.expressionConverter(right));
		}

		else{
			tNode = et.exptreeSetupSON(et.expressionConverter(right.substr(1,right.length()-2)));
		}

		std::vector<Node*> exprList = pkb->getExpressions(tNode->getValue());
		for (unsigned int i = 0 ; i < exprList.size() ; i++){
			Node* tNode2 = exprList.at(i);
			int compareResult = compare(tNode2,tNode);
			if(compareResult != 0){
				Node* parentNode = tNode2 ->getParent();
				if (firstChar != "_"){
					if(parentNode->getType() == assignment){
						if (left == "_"){
							result.push_back(parentNode);
						}
						else{
							if(parentNode->getLeftChild()->getVariable()->getName() == left){
								result.push_back(parentNode);
							}
						}
					}
				}
				else{
					if (left == "_"){
						result.push_back(parentNode);	
					}
					else{
						while(parentNode->getType()!= assignment){
							parentNode = parentNode ->getParent();
						}
						if(parentNode->getLeftChild()->getVariable()->getName() == left){
							result.push_back(parentNode);
						}
					}
				}
			}
		}
	}

	return result;
}

int compare(Node* p,Node* q){
   if(p == NULL && q == NULL){
	   return 1;
   }
   else if(p == NULL || q == NULL){
	   return 0;
   }
   else if(p->getValue() == q->getValue())   {
	   int s1=compare(p->getLeftChild(),q->getLeftChild());
	   int s2=compare(p->getRightChild(),q->getRightChild());
	   return s1*s2;
   }
   else
		return 0;
}