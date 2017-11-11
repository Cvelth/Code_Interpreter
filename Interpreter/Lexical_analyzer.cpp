#include "Interpreter.hpp"
#include "Default_lanuage.hpp"
void separate_comments_and_strings(std::list<Token> &source) {
	size_t commentary = 0;
	size_t string = 0;
	size_t chars = 0;
	for (auto it = source.begin(); it != source.end(); it++) {
		if (it->type == TokenType::unknown)
			for (size_t i = 0; i < (*it)->size(); i++) {
				if ((*it)->at(i) == lang::comment_start && !commentary && !string && !chars)
					commentary = i + 1;
				if ((*it)->at(i) == lang::comment_end && commentary) {
					Token prev = (*it)->substr(0, commentary - 1);
					Token temp = (*it)->substr(commentary, i - commentary);
					temp.type = TokenType::commentary;
					Token next = (*it)->substr(i + 1);
					source.insert(it, prev);
					source.insert(it, temp);
					source.insert(it, next);
					auto del = it;
					it--;
					source.erase(del);
					i = 0;
					commentary = 0;
				}
				if ((*it)->at(i) == lang::string_literal && !commentary && !chars)
					if (!string)
						string = i + 1;
					else {
						Token prev = (*it)->substr(0, string - 1);
						Token temp = (*it)->substr(string, i - string);
						temp.type = TokenType::string_literal;
						Token next = (*it)->substr(i + 1);
						source.insert(it, prev);
						source.insert(it, temp);
						source.insert(it, next);
						auto del = it;
						it--; it--;
						source.erase(del);
						i = 0;
						string = 0;
					}
					if ((*it)->at(i) == lang::chars_literal && !commentary && !string)
						if (!chars)
							chars = i + 1;
						else {
							Token prev = (*it)->substr(0, chars - 1);
							Token temp = (*it)->substr(chars, i - chars);
							temp.type = TokenType::string_literal;
							Token next = (*it)->substr(i + 1);
							source.insert(it, prev);
							source.insert(it, temp);
							source.insert(it, next);
							auto del = it;
							it--; it--;
							source.erase(del);
							i = 0;
							chars = 0;
						}
			}
		if (commentary)
			throw std::exception("Cannot determine the end of the commentary.");
		if (string)
			throw std::exception("Cannot determine the end of the string literal.");
	}
}
void split_on_separators(std::list<Token> &source, std::string const& separators) {
	for (auto it = source.begin(); it != source.end(); it++)
		if (it->type == TokenType::unknown)
			for (size_t i = 0; i < (*it)->size(); i++)
				if (separators.find((*it)->at(i)) != std::string::npos) {
					Token prev = (*it)->substr(0, i);
					Token next = (*it)->substr(i + 1);

					source.insert(it, prev);
					source.insert(it, next);
					auto del = it;
					it--; it--;
					source.erase(del);
					i = 0;
				}
}
void differenciate_separators(std::list<Token> &source, std::string const& separators) {
	for (auto it = source.begin(); it != source.end(); it++)
		if (it->type == TokenType::unknown)
			for (size_t i = 0; i < (*it)->size(); i++)
				if (separators.find((*it)->at(i)) != std::string::npos) {
					Token prev = (*it)->substr(0, i);
					Token temp = std::string{(*it)->at(i)};
					Token next = (*it)->substr(i + 1);

					source.insert(it, prev);
					source.insert(it, temp);
					source.insert(it, next);
					auto del = it;
					it--; it--;
					source.erase(del);
					i = 0;
				}
}
void unite_operators(std::list<Token> &source) {
	auto it1 = source.begin();
	auto it2 = it1; it2++;
	for (; it2 != source.end(); it1++, it2++)
		if (lang::operators_2.find(it1->name + it2->name) != lang::operators_2.end()) {
			Token temp = it1->name + it2->name;
			temp.type = TokenType::binary_operator;

			source.insert(it2, temp);
			source.erase(it1);
			source.erase(it2);
			it1 = source.begin();
			it2 = it1; it2++;
		}
}
bool check_name(std::string const& source) {
	if (!isalpha(source[0]) && source[0] != '_') return false;
	for (size_t i = 1; i < source.size(); i++)
		if (!isalnum(source[i]) && source[i] != '_') return false;
	return true;
}
bool check_literal(std::string const& source) {
	for (size_t i = 0; i < source.size(); i++)
		if (!isdigit(source[i])) return false;
	return true;
}
TokenType classify_name(std::string const& source) {
	if (check_name(source))
		return TokenType::type_name;
	if (check_literal(source))
		return TokenType::int_literal;

	if (source[0] == lang::variable)
		if (check_name(source.substr(1)))
			return TokenType::variable_name;
	if (source[0] == lang::list)
		if (check_name(source.substr(1)))
			return TokenType::list_name;
	if (source[0] == lang::hash)
		if (check_name(source.substr(1)))
			return TokenType::hash_name;

	throw std::exception(("Unsupported token: " + source).c_str());
}
TokenType classify_token(std::string const& name) {
	if (lang::reserved_words.find(name) != lang::reserved_words.end())
		return TokenType::reserved_word;
	else if (name == ";")
		return TokenType::semicolon;
	else if (lang::operators_1.find(name) != lang::operators_1.end()
			 || lang::operators_2.find(name) != lang::operators_2.end())
		return TokenType::binary_operator;
	else if (name == "(" || name == ")" || name == "[" || name == "]" || name == "{" || name == "}")
		return TokenType::bracket;
	else
		return classify_name(name);
}
void classify_tokens(std::list<Token> &source) {
	for (auto it = source.begin(); it != source.end(); it++)
		if (it->type == TokenType::unknown)
			it->type = classify_token(it->name);
}
void clean_tokens(std::list<Token> &source) {
	for (auto it = source.begin(); it != source.end(); it++)
		if (it->name == "") {
			auto del = it;
			if (it != source.begin()) {
				it--;
				source.erase(del);
			} else {
				source.erase(del);
				it = source.begin();
			}
		}
}
std::list<Token> lexical_analysis(std::string const& source) {
	if (source == "")
		throw std::exception("There's nothing to analyse. No code was passed.");
	std::list<Token> res{source};
	separate_comments_and_strings(res);
	split_on_separators(res, " \t\n");
	differenciate_separators(res, ";=>.-()[]{}+,");
	clean_tokens(res);
	unite_operators(res);
	classify_tokens(res);
	return res;
}