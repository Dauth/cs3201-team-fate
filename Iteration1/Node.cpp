#include "Node.h"

Node::Node (synt_type st, std::string id) {
		SYNT_TYPE = st;
		identifier = id;
}

Node* Node::getParent() {
		return parent;
}

void Node::setVar(Variable* v) {
	var = v;
}
