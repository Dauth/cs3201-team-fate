#include "ParentFunctions.h";
#include "ParentsTable.h"
#include <vector>;

ParentsTable pt;
std::vector<Node*> getParentsFor(Node* parent){
	return pt.getChildren(parent);
}

Node* getParentsOf(Node* child){
	return child->getParent();
}

std::vector<Node*> getParentsStarFor(Node* parent){
	std::vector<Node*> childList;
	std::vector<Node*> tempList;
	childList = getParentsFor(parent);
	tempList = getParentsFor(parent);
	for (unsigned i=0; i<tempList.size(); i++)
		//if(tempList.at(i) -> getType().compare("while")){

	}
	
}