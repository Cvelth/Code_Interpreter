#include "Interpreter.hpp"
std::string interpret(std::string const& source) {
	auto lexemes = lexical_analysis(source);
	auto syntax = syntax_analysis(lexemes);
	bool results = semantic_analysis(syntax);
	if (!results)
		throw std::exception("Semantic analysis was unsuccessful");
	return "Result wasn't implemented, yet.";
}