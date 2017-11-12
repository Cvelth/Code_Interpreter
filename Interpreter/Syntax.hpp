#pragma once
#include <string>
#include "Token.hpp"
#include <memory>
struct Node : public Token {
	std::shared_ptr<Node> left;
	std::shared_ptr<Node> right;
	Node(std::string const& name, TokenType type = TokenType::unknown, std::shared_ptr<Node> left = nullptr, std::shared_ptr<Node> right = nullptr)
		: Token(name, type), left(left), right(right) {}
	Node(TokenType type = TokenType::unknown, std::shared_ptr<Node> left = nullptr, std::shared_ptr<Node> right = nullptr)
		: Node("", type, left, right) {}
	Node(Token const& token, std::shared_ptr<Node> left = nullptr, std::shared_ptr<Node> right = nullptr)
		: Node(token.name, token.type, left, right) {}
	
	bool operator==(Node const& other) const {
		return name == other.name && type == other.type;
	}
	bool operator<(Node const& other) const {
		return name < other.name;
	}
};
#include <list>
#include <set>
class Syntax {
public:
	std::set<Node> typenames;
	std::set<Node> variables;
	std::set<Node> constants;
	std::shared_ptr<Node> graph;
	Syntax(std::list<Token> const& source);
	bool check_semantics() const;
private:
	std::list<std::shared_ptr<Node>> parse_brackets(std::list<std::shared_ptr<Node>> source, char bracket_type = 0);
	std::shared_ptr<Node> parse_graph(std::list<std::shared_ptr<Node>> source);
	std::shared_ptr<Node> parse_operators(std::list<std::shared_ptr<Node>> source);
	bool parse_semantics(std::shared_ptr<Node> syntax, TokenType expectedType = TokenType::unknown) const;
	bool is_string_convertable(std::shared_ptr<Node> syntax) const;
};