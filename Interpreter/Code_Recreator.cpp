#include "Interpreter.hpp"
#include <sstream>
size_t variable_index = 0;
std::ostream& operator<<(std::ostream &s, std::set<Node> const& set) {
	for (auto it : set) {
		if (it.type == TokenType::int_literal)
			s << "_inner_name_" << variable_index++ << ": \t.word\t" << it.name << '\n';
		else if (it.type == TokenType::string_literal)
			s << "_inner_name_" << variable_index++ << ": \t.asciz\t\"" << it.name << "\"\n";
		else if (it.type == TokenType::variable_name)
			s << it.name << "_var:\t.word\t0\n";
		else if (it.type == TokenType::list_name)
			s << it.name << "_list:\t.word\t0\n";
		else if (it.type == TokenType::hash_name)
			s << it.name << "_hash:\t.word\t0\n";
	}
	return s;
}
std::ostream& operator<<(std::ostream &s, Node const& set) {

	return s;
}
std::string code_recreation(Syntax const& syntax, bool semantic_result) {
	if (!semantic_result)
		throw std::exception("Semantic_analysis was unsuccessful. Terminating...");

	std::stringstream ret;

	if (!syntax.variables.empty()) {
		ret << "@Variables used in the file:\n";
		ret << syntax.variables;
	}
	if (!syntax.constants.empty()) {
		ret << "@Constants used in the file:\n";
		ret << syntax.constants;
	}
	ret << *syntax.graph;

	return ret.str();
}