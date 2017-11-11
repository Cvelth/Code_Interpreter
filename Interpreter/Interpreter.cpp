#include "Interpreter.hpp"
std::string interpret(std::string const& source) {
	auto lexemes = lexical_analisys(source);
	//throw std::exception(("Unimplemented function was called with source = " + source).c_str());
	return "";
}