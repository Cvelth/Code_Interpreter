#pragma once
#include "dll.hpp"
#include <string>
enum class TokenType {
	unknown,
};
class Token {
	std::string name;
	TokenType type;
	Token(std::string const& name, TokenType type) : name(name), type(type) {}
};