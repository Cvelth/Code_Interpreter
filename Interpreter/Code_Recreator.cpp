#include "Interpreter.hpp"
#include <sstream>
#include <map>
std::ostream& operator<<(std::ostream &s, Node const& node);
size_t variable_index = 0;
std::string const sh("\t");
std::map<std::string, size_t> constants;
std::map<std::string, size_t> inner_constants;
std::string current_package;
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
void insert_parameter(std::ostream &s, Node const& node, size_t &r) {
	if (node.type == TokenType::binary_operator && node.name == ",") {
		insert_parameter(s, *node.left, r);
		insert_parameter(s, *node.right, r);
	} else if (node.type == TokenType::binary_operator && node.name == "=>") {
		s << sh << "ldr r" << r++ << ", =" << "_constant_" << constants[node.right->name] << '\n';
	}
}
void generate_list(std::ostream &s, Node const& node, size_t &l) {
	if (node.type == TokenType::binary_operator && node.name == ",") {
		generate_list(s, *node.left, l);
		generate_list(s, *node.right, l);
	} else if (node.type == TokenType::binary_operator && node.name == "->") {
			size_t r = 0;
			insert_parameter(s, *node.right->right, r);
			s << sh << "bl " << node.left->name << "_" << node.right->name << "\n";
			s << sh << "str r" << l++ << ", =" << node.left->name << "_var\n";
		}
}
void string_concat(std::ostream &s, Node const& node, size_t l) {
	inner_constants.insert(std::make_pair("%s", variable_index++));
	if (node.left->type == TokenType::binary_operator && node.left->name == ".")
		string_concat(s, *node.left, l + 1);
	if (node.right->type == TokenType::binary_operator && node.right->name == ".")
		string_concat(s, *node.right, l + 1);
	if (node.left->type == TokenType::string_literal) {
		inner_constants.insert(std::make_pair("\"\"", variable_index++));
		s << "\n" << sh << "mov r0, =" << "_constant_" << constants[node.left->name] << '\n'
			<< sh << "mov r2, =" << "_inner_constant_" << inner_constants["\"\""] << '\n'
			<< sh << "mov r4, #128\n"
			<< sh << "loop_l_" << l << ":\n"
			<< sh << sh << "ldrb r3, [r0], #1\n"
			<< sh << sh << "strb r3, [r2], #1\n"
			<< sh << sh << "subs r4, #1\n"
			<< sh << "bne loop_l_" << l << "\n";
	}
	if (node.right->type == TokenType::string_literal) {
		inner_constants.insert(std::make_pair("\"\"", variable_index++));
		s << "\n" << sh << "mov r0, =" << "_constant_" << constants[node.right->name] << '\n'
			<< sh << "mov r2, =" << "_inner_constant_" << inner_constants["\"\""] << '\n'
			<< sh << "mov r4, #128\n"
			<< sh << "loop_r_" << l << ":\n"
			<< sh << sh << "ldrb r3, [r0], #1\n"
			<< sh << sh << "strb r3, [r2], #1\n"
			<< sh << sh << "subs r4, #1\n"
			<< sh << "bne loop_r_" << l << "\n";
	}
}
std::ostream& operator<<(std::ostream &s, Node const& node) {
	if (node.type == TokenType::semicolon) {
		if (node.left) s << *node.left << "\n";
		if (node.right) s << *node.right;
	} else if (node.type == TokenType::binary_operator) {
		if (node.name == ".") {
			string_concat(s, node, 0);
		} else if (node.name == "+=") {
			if (node.right->type == TokenType::variable_name)
				s << '\n' << sh << "ldr r0, " << node.left->name << "_var\n"
					<< sh << "ldr r1, " << node.right->name << "_var\n"
					<< sh << "adds r0, r1\n"
					<< sh << "str r0, " << node.left->name << "_var\n";
			else if (node.right->type == TokenType::int_literal)
				s << sh << "ldr r0, " << node.left->name << "_var\n"
					<< sh << "ldr r1, =_constant_" << constants[node.right->name] << "\n"
					<< sh << "adds r0, r1\n"
					<< sh << "str r0, " << node.left->name << "_var\n";
		} else if (node.name == "=") {
			size_t r = 4;
			generate_list(s, *node.right->right, r);
			s << "\n" << sh << "ldr r0, =" << node.left->name << "_var\n"
				<< sh << "mov r4, [r0], #1\n"
				<< sh << "mov r5, [r0], #1\n"
				<< sh << "mov r6, [r0], #1\n"
				<< sh << "mov r7, [r0], #1\n"
				<< sh << "mov r8, [r0], #1\n";
		}
	} else if (node.type == TokenType::reserved_word) {
		if (node.name == "print") {
			if (node.right->type == TokenType::string_literal) {
				inner_constants.insert(std::make_pair("%s", variable_index++));
				s << sh << "ldr r0, =" << "_inner_constant_" << inner_constants["%s"]
					<< "\n" << sh << "mov r1, =_constant_" << constants[node.right->name]
					<< "\n" << sh << "bl printf\n";
			} else if (node.right->type == TokenType::binary_operator && node.right->name == ".") {
				s << *node.right
					<< "\n" << sh << "ldr r0, =" << "_inner_constant_" << inner_constants["%s"]
					<< "\n" << sh << "mov r1, =_inner_constant_" << inner_constants["\"\""]
					<< "\n" << sh << "bl printf\n";
			}
		} else if (node.name == "package") {
			s << "@package " << current_package << " start here.\n";
		} else if (node.name == "sub") {
			s << ".extern " << current_package << "_" << node.left->name << '\n';
			s << current_package << "_" << node.left->name << ":\n"
				<< sh << "@Subroutine arguments:\n"
				<< sh << sh << "@r0 - " << current_package << "-pointer\n"
				<< sh << sh << "@r1 - pointer to the argument list {@_}\n"
				<< sh << "push {r4, lr}\n"
				<< *node.right
				<< sh << "pop {r4, pc}\n";
		} else if (node.name == "my") {
			if (node.right->type == TokenType::variable_name)
				s << '\n' << sh << "ldr r0, " << node.right->name << "_var\n"
					<< sh << "str r0, " << node.left->name << "_var\n";
			else if (node.right->type == TokenType::int_literal)
				s << "ldr r0, =_constant_" << constants[node.right->name] << "\n"
				<< "str r0, " << node.left->name << "_var\n";
			else if (node.right->type == TokenType::reserved_word && node.right->name == "shift") {
				inner_constants.insert(std::make_pair(current_package, variable_index++));
				s << '\n' << sh << "ldr r0, " << "_inner_constant_" << inner_constants[current_package] << "\n"
					<< sh << "str r0, " << node.left->name << "_var";
			} else if (node.right->type == TokenType::bracket && node.right->name == "{}") {
				s << '\n' << sh << "ldr r0, " << (node.right->right->name == "_" ? "r1" : ("=" + node.right->right->name + "_list")) << '\n'
					<< sh << "str r0, " << node.left->name << "_var";
			} else if (node.right->type == TokenType::binary_operator && node.right->name == "->") {
				size_t r = 0;
				insert_parameter(s, *node.right->right->right, r);
				s << sh << "bl " << node.right->left->name << "_" << node.right->right->name << "\n";
				s << sh << "str r0, =" << node.left->name << "_var";
			}
		} else if (node.name == "bless") {
			s << '\n' << sh << "ldr r0, =" << node.right->name << "_var\n"
				<< sh << "str r0, " << node.left->name << "_var\n";
		} else if (node.name == "return") {
			if (node.right->type == TokenType::variable_name)
				s << sh << "ldr r0, " << node.right->name << "_var\n";
			else if (node.right->type == TokenType::binary_operator && node.right->name == "->" && node.right->right->name == "{}" && node.right->right->right->type == TokenType::type_name) {
				s << '\n' << sh << "ldr r1, " << node.right->left->name << "_var\n";
				s << sh << "ldr r2, " << node.right->right->right->name << "_var\n";
				s << sh << "add r1, r2\n" << sh << "mov r0, =r1";
			} else if (node.right->type == TokenType::binary_operator && node.right->name == ".") {
				s << *node.right
					<< '\n' << sh << "ldr r1, _inner_constant_" << inner_constants["%s"] << "\n"
					<< sh << "ldr r2, =_inner_constant_" << inner_constants["\"\""] << "\n"
					<< sh << "add r1, r2\n" << sh << "mov r0, =r1";
			}
		} else if (node.name == "use") {
			if (node.left->name == "lib")
				s << ".extern '" << node.right->name << "'";
			else
				s << ".extern " << node.left->name;
		} else if (node.name == "our") {
			if (node.left->name == "ISA") {
				s << ".extern " << node.right->name << '\n';
			}
		} else if (node.name == "foreach") {
			s << sh << "ldr r5, " << node.right->left->right->name << "_list\n"
				<< sh << "mov r6, #0\n"
				<< sh << "loop_f_" << variable_index++ << ":\n"
				<< sh << sh << "mov r0, [r5, r6]\n"
				<< sh << sh << "bl " << "Point_" << node.right->right->right->left->right->right->name << "\n"
				<< sh << sh << "mov r1, r0\n"
				<< sh << sh << "ldr r0, =" << "_inner_constant_" << inner_constants["%s"] << "\n"
				<< sh << sh << "bl printf\n"
				<< sh << sh << "add r6, #1\n"
				<< sh << sh << "bne loop_f_" << variable_index - 1 << "\n";
		}
	} else if (node.type == TokenType::int_literal && node.name == "1") {
		s << "@package " << current_package << " ends here.";
		current_package = "";
	} else if (node.type == TokenType::bracket && node.name == "{}")
		s << *node.right;
	return s;
}
std::string code_recreation(Syntax const& syntax, bool semantic_result, bool print_comments) {
	if (!semantic_result)
		throw std::exception("Semantic_analysis was unsuccessful. Terminating...");

	std::stringstream ret;
	constants.clear();
	inner_constants.clear();
	current_package = "";
	
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
		<< "\n.extern printf\n";
	if (syntax.graph->left->type == TokenType::reserved_word && syntax.graph->left->name == "package") {
		current_package = syntax.graph->left->right->name;
		ret << *syntax.graph;
	} else {
		ret << ".global main\n\n"
			<< "main:\n"
			<< sh << "push {ip, lr}\n\n";

		ret << *syntax.graph;

		ret << sh << "pop {ip, pc}\n";
	}

	if (!inner_constants.empty()) {
		ret << "\n.data\n";
		if (print_comments)
			ret << "@Constants used in the interpreter:\n";
		for (auto it : inner_constants) 
			ret << "_inner_constant_" << it.second << ":\t.asciz\t\"" << it.first << "\"\n";
	}

	if (print_comments)
		ret << "\n@End of the code listing.";

	return ret.str();
}