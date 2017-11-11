#include "Interpreter.hpp"
std::list<Token> lexical_analisys(std::string const& source) {
	

	return std::list<Token>();
}
std::string interpret(std::string const& source) {
	auto lexemes = lexical_analisys(source);
	throw std::exception(("Unimplemented function was called with source = " + source).c_str());
}