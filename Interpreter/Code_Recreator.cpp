#include "Interpreter.hpp"
#include <sstream>
#include <map>
size_t variable_index = 0;
std::string const sh("\t");
std::map<std::string, size_t> constants;
std::map<std::string, size_t> inner_constants;
std::ostream& operator<<(std::ostream &s, std::set<Node> const& set) {
	for (auto it : set) {
		if (it.type == TokenType::int_literal) {
			constants.insert(std::make_pair(it.name, variable_index));
			s << "_constant_" << variable_index++ << ": \t.word\t" << it.name << '\n';
		} else if (it.type == TokenType::string_literal) {
			constants.insert(std::make_pair(it.name, variable_index));
			s << "_constant_" << variable_index++ << ": \t.asciz\t\"" << it.name << "\"\n";
		} else if (it.type == TokenType::variable_name)
			s << it.name << "_var:\t.word\t0\n";
		else if (it.type == TokenType::list_name)
			s << it.name << "_list:\t.word\t0\n";
		else if (it.type == TokenType::hash_name)
			s << it.name << "_hash:\t.word\t0\n";
	}
	return s;
}
std::ostream& operator<<(std::ostream &s, Node const& node) {
	if (node.type == TokenType::semicolon) {
		if (node.left) s << *node.left;
		if (node.left) s << "\n";
		if (node.right) s << *node.right;
		if (node.right) s << "\n";
	}  else if (node.type == TokenType::reserved_word) {
		if (node.name == "print") {
			if (node.right->type == TokenType::string_literal) {
				inner_constants.insert(std::make_pair("%s", variable_index++));
				s << sh << "mov r0, =" << "_inner_constant_" << inner_constants["%s"]
					<< "\n" << sh << "mov r1, =_constant_" << constants[node.right->name]
					<< "\n" << sh << "bl printf\n";
			}
		}
	}
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

	if (!syntax.variables.empty() || !syntax.constants.empty())
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
		<< sh << "push {ip, lr}\n\n";

	ret << *syntax.graph;

	ret << sh << "pop {ip, pc}\n";

	if (!inner_constants.empty()) {
		ret << "\n.data\n";
		if (print_comments)
			ret << "@Constants used in the interpreter:\n";
		for (auto it : inner_constants) 
			ret << "_inner_constant_" << it.second << ":\t.asciz\t\"" << it.first << "\n";
	}

	if (print_comments)
		ret << "\n@End of the code listing.";

	return ret.str();
}