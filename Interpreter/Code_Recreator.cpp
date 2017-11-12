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
std::string code_recreation(Syntax const& syntax, bool semantic_result, bool print_comments) {
	if (!semantic_result)
		throw std::exception("Semantic_analysis was unsuccessful. Terminating...");

	std::stringstream ret;
	
	if (print_comments)
		ret << "@This file was automatically created.\n"
			<< "@Do not rewrite this file as all the changes\n"
			<< "@will be lost on the next compilation.\n\n"
			<< "@The code was interpreted from Perl(R) language\n" 
			<< "@using Code_Interpreter v0.1.62dev.\n";

	ret << "\n.data\n";
	if (!syntax.variables.empty()) {
		if (print_comments)
			ret << "\n@Variables used in the file:\n";
		ret << syntax.variables;
	}
	if (!syntax.constants.empty()) {
		if (print_comments)
			ret << "\n@Constants used in the file:\n";
		ret << syntax.constants;
	}
	ret << "\n.text"
		<< "\n.extern printf"
		<< "\n.global main\n\n"
		<< "main:\n"
		<< "push {ip, lr}\n\n";

	ret << *syntax.graph;

	ret << "\n\npop {ip, pc}\n\n";
	if (print_comments)
		ret << "@End of the code listing.";

	return ret.str();
}