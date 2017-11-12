#include "Interpreter.hpp"
#include <sstream>
std::ostream& operator<<(std::ostream &s, std::set<Node> const& set) {

	return s;
}
std::ostream& operator<<(std::ostream &s, Node const& set) {

	return s;
}
std::string code_recreation(Syntax const& syntax, bool semantic_result) {
	if (!semantic_result)
		throw std::exception("Semantic_analysis was unsuccessful. Terminating...");

	std::stringstream ret;

	ret << "@Variables used in the file:\n";
	ret << syntax.variables;
	ret << *syntax.graph;

	return ret.str();
}