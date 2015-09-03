#ifndef FollowsFunctions_H
#define FollowsFunctions_H


public:
	Node* getFollows(Node* node);
	Node* getFollowsBy(Node* node);
	std::vector<Node*> getFollowsStar(Node* node);
	std::vector<Node*> getFollowsStarBy(Node* node);

#endif