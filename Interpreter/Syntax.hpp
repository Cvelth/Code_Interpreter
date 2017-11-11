#pragma once
#include <string>
#include "Token.hpp"
struct Node : public Token {
	using Token::Token;

	Node *left;
	Node *right;
};
#include <list>
#include <set>
struct Syntax {
	std::set<std::string> variables;
	std::set<std::string> constants;
	std::list<Node*> operations;
};