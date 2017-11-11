#include "Interpreter.hpp"
std::list<Lexeme> lexical_analisys(std::string const& source) {
	

	return std::list<Lexeme>();
}
std::string interpret(std::string const& source) {
	auto lexemes = lexical_analisys(source);
	throw std::exception(("Unimplemented function was called with source = " + source).c_str());
}