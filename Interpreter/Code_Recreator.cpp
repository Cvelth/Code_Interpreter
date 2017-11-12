#include "Interpreter.hpp"
std::string code_recreation(Syntax const& syntax, bool semantic_result) {
	if (!semantic_result)
		throw std::exception("Semantic_analysis was unsuccessful. Terminating...");

	return std::string();
}