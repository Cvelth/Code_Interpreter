#include "Interpreter.hpp"
std::list<std::shared_ptr<Node>> convert_to_nodes(std::list<Token> const& source) {
	std::list<std::shared_ptr<Node>> ret;
	for (auto it : source)
		ret.push_back(std::make_shared<Node>(it));
	return ret;
}
std::list<std::shared_ptr<Node>> Syntax::parse_brackets(std::list<std::shared_ptr<Node>> source, char bracket_type) {
	for (auto it = source.begin(); it != source.end(); it++) {
		if ((*it)->type == TokenType::bracket)
			if ((*it)->name == "(" || (*it)->name == "[" || (*it)->name == "{") {
				auto temp = it; temp++;
				auto nodes = parse_brackets({temp, source.end()}, (*it)->name[0]);
				source = {source.begin(), it};
				source.insert(source.end(), nodes.begin(), nodes.end());
				it = source.begin();
			} else if (bracket_type && ((*it)->name == ")" || (*it)->name == "]" || (*it)->name == "}")) {
				auto ret = std::make_shared<Node>("", TokenType::bracket);
				if ((*it)->name == ")" && bracket_type == '(')
					ret->name = "()";
				else if ((*it)->name == "}" && bracket_type == '{')
					ret->name = "{}";
				else if ((*it)->name == "]" && bracket_type == '[')
					ret->name = "[]";

				if (it == source.begin())
					ret->right = nullptr;
				else
					ret->right = parse_graph(std::list<std::shared_ptr<Node>>{ source.begin(), it });

				source = {++it, source.end()};
				source.push_front(ret);
				return source;
			}
	}
	return source;
}
std::shared_ptr<Node> Syntax::parse_operators(std::list<std::shared_ptr<Node>> source) {
	if (source.size() == 0) {
		return nullptr;
	} else if (source.size() == 1) {
		if (source.front()->type == TokenType::variable_name || source.front()->type == TokenType::list_name || source.front()->type == TokenType::hash_name)
			variables.insert(*source.front());
		else if (source.front()->type == TokenType::int_literal || source.front()->type == TokenType::string_literal)
			constants.insert(*source.front());
		else if (source.front()->type == TokenType::type_name)
			typenames.insert(*source.front());
		else if (source.front()->type == TokenType::reserved_word && (source.front()->name == "shift")) {

		} else if (source.front()->type == TokenType::bracket && (source.front()->name == "()" || source.front()->name == "{}")) {

		} else
			throw std::exception("Parsing leaf is beint terminated with unsupported simple structure. Please recheck code structure.");
		return source.front();
	} else {
		for (auto it = source.begin(); it != source.end(); it++) {
			if ((*it)->type == TokenType::semicolon) {
				auto ret = std::make_shared<Node>((*it)->name, TokenType::semicolon);
				if (it != source.begin())
					ret->left = parse_graph(std::list<std::shared_ptr<Node>>{source.begin(), it});
				if (it != --source.end())
					ret->right = parse_graph(std::list<std::shared_ptr<Node>>{++it, source.end()});
				return ret;
			}
		}
		for (auto it = source.begin(); it != source.end(); it++) {
			if ((*it)->type == TokenType::binary_operator && (*it)->name == ",") {
				auto ret = std::make_shared<Node>((*it)->name, TokenType::binary_operator);
				if (it != source.begin())
					ret->left = parse_graph(std::list<std::shared_ptr<Node>>{source.begin(), it});
				if (it != --source.end())
					ret->right = parse_graph(std::list<std::shared_ptr<Node>>{++it, source.end()});
				return ret;
			}
		}
		for (auto it = source.begin(); it != source.end(); it++) {
			if ((*it)->type == TokenType::commentary) {
				source.erase(it);
				it = source.begin();
			} 
			if ((*it)->type == TokenType::binary_operator && (*it)->name != ",") {
				auto ret = std::make_shared<Node>((*it)->name, TokenType::binary_operator);
				ret->left = parse_graph(std::list<std::shared_ptr<Node>>{source.begin(), it});
				ret->right = parse_graph(std::list<std::shared_ptr<Node>>{++it, source.end()});
				return ret;
			} else if ((*it)->type == TokenType::reserved_word) {
				if ((*it)->name == "print" || (*it)->name == "return") {
					auto ret = std::make_shared<Node>((*it)->name, TokenType::reserved_word);
					ret->right = parse_graph(std::list<std::shared_ptr<Node>>{++it, source.end()});
					return ret;
				} else if ((*it)->name == "package") {
					auto ret = std::make_shared<Node>((*it)->name, TokenType::reserved_word);
					++it;
					if ((*it)->type != TokenType::type_name)
						throw std::exception(("Incorrect name of a package was passed:" + (*it)->name).c_str());
					ret->right = parse_graph(std::list<std::shared_ptr<Node>>{it, source.end()});
					return ret;
				} else if ((*it)->name == "sub") {
					auto ret = std::make_shared<Node>((*it)->name, TokenType::reserved_word);
					ret->left = *(++it);
					if ((*it)->type != TokenType::type_name)
						throw std::exception(("Incorrect name of a subroutine was passed:" + (*it)->name).c_str());
					++it;
					if ((*it)->type != TokenType::bracket && (*it)->name == "{}")
						throw std::exception(("{}-brackets were expected but \"" + (*it)->name + "\" was fount instead.").c_str());
					ret->right = parse_graph(std::list<std::shared_ptr<Node>>{++it, source.end()});
					return ret;
				} else if ((*it)->name == "bless") {
					auto ret = std::make_shared<Node>((*it)->name, TokenType::reserved_word);
					ret->left = *(++it);
					if ((*it)->type != TokenType::variable_name)
						throw std::exception(("Incorrect parameter to a bless structure was passed:" + (*it)->name).c_str());
					++it;
					if ((*it)->type != TokenType::binary_operator || (*it)->name != ",")
						throw std::exception(((*it)->name + " found instead of ',' in \"bless\" structure.").c_str());
					++it;
					if ((*it)->type != TokenType::variable_name)
						throw std::exception(("Incorrect parameter to a bless structure was passed:" + (*it)->name).c_str());
					ret->right = parse_graph(std::list<std::shared_ptr<Node>>{it, source.end()});
					return ret;
				} else if ((*it)->name == "my") {
					auto ret = std::make_shared<Node>((*it)->name, TokenType::reserved_word);
					ret->left = *(++it);
					if ((*it)->type != TokenType::variable_name)
						throw std::exception(((*it)->name + " found instead of variable name in \"my\" structure.").c_str());
					++it;
					if ((*it)->type != TokenType::binary_operator || (*it)->name != "=")
						throw std::exception(((*it)->name + " found instead of '=' in \"my\" structure.").c_str());
					ret->right = parse_graph(std::list<std::shared_ptr<Node>>{++it, source.end()});
					return ret;
				} else if ((*it)->name == "use") {
					auto ret = std::make_shared<Node>((*it)->name, TokenType::reserved_word);
					ret->left = *(++it);
					if ((*it)->type != TokenType::type_name)
						throw std::exception(((*it)->name + " found instead of library name in \"use\" structure.").c_str());
					if ((*it)->name == "lib") {
						++it;
						if ((*it)->type != TokenType::string_literal)
							throw std::exception(((*it)->name + " found instead of string literal in \"use lib\" structure.").c_str());
						ret->right = parse_graph(std::list<std::shared_ptr<Node>>{it, source.end()});
					}
					return ret;
				}
			}
		}
	}
	if (source.size() == 2)
		if (source.front()->type == TokenType::type_name && source.back()->type == TokenType::bracket && source.back()->name == "()")
			return std::make_shared<Node>(source.front()->name, TokenType::function, source.front(), source.back()->right);
	throw std::exception("Some operations are unaccessible for the parser. Recheck code structure.");
}
std::shared_ptr<Node> Syntax::parse_graph(std::list<std::shared_ptr<Node>> source) {
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