#pragma once
#include "dll.hpp"
#include <string>
enum class TokenType {
	unknown,
	commentary,
	string_literal,
};
struct Token {
	std::string name;
	TokenType type;
	Token(std::string const& name, TokenType type = TokenType::unknown) : name(name), type(type) {}
	std::string* operator->() {	return &name; }
	std::string const* operator->() const {	return &name; }
};