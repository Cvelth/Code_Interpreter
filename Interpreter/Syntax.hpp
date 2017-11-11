#pragma once
#include <string>
#include "Token.hpp"
struct Node : public Token {
	Node *left;
	Node *right;
	Node(std::string const& name, TokenType type = TokenType::unknown, Node *left = nullptr, Node *right = nullptr)
		: Token(name, type), left(left), right(right) {}
	Node(TokenType type = TokenType::unknown, Node *left = nullptr, Node *right = nullptr)
		: Token("", type), left(left), right(right) {}
	Node(Token const& token, Node *left = nullptr, Node *right = nullptr) 
		: Node(token.name, token.type, left, right) {}
};
#include <list>
#include <set>
class Syntax {
public:
	std::set<Node*> variables;
	std::set<Node*> constants;
	Node* graph;
	Syntax(std::list<Token> const& source);
private:
	std::list<Node*> parse_brackets(std::list<Node*> source, char bracket_type = 0);
	Node* parse_graph(std::list<Node*> source);
	Node* parse_operators(std::list<Node*> source);
};