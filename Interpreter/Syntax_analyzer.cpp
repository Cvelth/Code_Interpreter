#include "Interpreter.hpp"
std::list<Node*> convert_to_nodes(std::list<Token> const& source) {
	std::list<Node*> ret;
	for (auto it : source)
		ret.push_back(new Node(it));
	return ret;
}
std::list<Node*> Syntax::parse_brackets(std::list<Node*> source) {
	for (auto it = source.begin(); it != source.end(); it++) {
		if ((*it)->type == TokenType::bracket)
			if ((*it)->name == "(" || (*it)->name == "[" || (*it)->name == "{") {
				auto temp = it; temp++;
				auto nodes = parse_brackets({temp, source.end()});
				source = {source.begin(), it};
				source.insert(source.end(), nodes.begin(), nodes.end());
				it = source.begin();
			} else if ((*it)->name == ")" || (*it)->name == "]" || (*it)->name == "}") {
				Node *ret = new Node("", TokenType::bracket);
				if ((*it)->name == ")")
					ret->name = "()";
				else if ((*it)->name == "}")
					ret->name = "{}";
				else if ((*it)->name == "]")
					ret->name = "[]";

				if (it == source.begin())
					ret->right = nullptr;
				else
					ret->right = parse_graph(std::list<Node*>{ source.begin(), it });
			}
	}
	return source;
}
Node* Syntax::parse_operators(std::list<Node*> source) {
	if (source.size() == 0) {
		return nullptr;
	} else if (source.size() == 1) {
		if (source.front()->type == TokenType::variable_name || source.front()->type == TokenType::list_name || source.front()->type == TokenType::hash_name)
			variables.insert(source.front());
		else if (source.front()->type == TokenType::int_literal || source.front()->type == TokenType::string_literal)
			constants.insert(source.front());
		else
			throw std::exception("Parsing leaf is beint terminated with unsupported simple structure. Please recheck code structure.");
		return source.front();
	} else {
		for (auto it = source.begin(); it != source.end(); it++) {
			if ((*it)->type == TokenType::semicolon) {
				auto ret = new Node((*it)->name,
									TokenType::semicolon);
				if (it != source.begin())
					ret->left = parse_graph(std::list<Node*>{source.begin(), it});
				if (it != --source.end())
					ret->right = parse_graph(std::list<Node*>{++it, source.end()});
				return ret;
			}
		}
		for (auto it = source.begin(); it != source.end(); it++) {
			if ((*it)->type == TokenType::commentary) {
				source.erase(it);
				it = source.begin();
			} 
			if ((*it)->type == TokenType::binary_operator) {
				auto ret = new Node((*it)->name, TokenType::binary_operator);
				ret->left = parse_graph(std::list<Node*>{source.begin(), it});
				ret->right = parse_graph(std::list<Node*>{++it, source.end()});
				return ret;
			} else if ((*it)->type == TokenType::reserved_word) {
				if ((*it)->name == "print") {
					auto ret = new Node((*it)->name, TokenType::reserved_word);
					ret->right = parse_graph(std::list<Node*>{++it, source.end()});
					return ret;
				}
			}
		}
	}
	throw std::exception("Some operations are unaccessible for the parser. Recheck code structure.");
}
Node* Syntax::parse_graph(std::list<Node*> source) {
	if (source.size() != 0)
		return parse_operators(parse_brackets(source));
	else
		return nullptr;
}

Syntax::Syntax(std::list<Token> const& source) {
	graph = {parse_graph(convert_to_nodes(source))};
}
Syntax syntax_analysis(std::list<Token> const& source) {
	return Syntax(source);
}