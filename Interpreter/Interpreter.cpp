#include "Interpreter.hpp"
std::string interpret(std::string const& source) {
	auto lexemes = lexical_analysis(source);
	auto syntax = syntax_analysis(lexemes);
	//throw std::exception(("Unimplemented function was called with source = " + source).c_str());
	return "";
}