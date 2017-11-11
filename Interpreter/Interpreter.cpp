#include "Interpreter.hpp"
#include "Perl.hpp"
using namespace lang;
void separate_comments_and_strings(std::list<Token> &source) {
	size_t commentary = 0;
	size_t string = 0;
	size_t chars = 0;
	for (auto it = source.begin(); it != source.end(); it++) {
		if (it->type == TokenType::unknown)
			for (size_t i = 0; i < (*it)->size(); i++) {
				if ((*it)->at(i) == comment_start && !commentary && !string && !chars)
					commentary = i + 1;
				if ((*it)->at(i) == comment_end && commentary) {
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
				if ((*it)->at(i) == string_literal && !commentary && !chars)
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
				if ((*it)->at(i) == chars_literal && !commentary && !string)
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
void classify_tokens(std::list<Token> &source) {
	for (auto it = source.begin(); it != source.end(); it++)
		if (it->type == TokenType::unknown) {
			if (lang::reserved_words.find(it->name) != lang::reserved_words.end())
				it->type = TokenType::reserved_word;
			else if (it->name == ";")
				it->type = TokenType::semicolon;
		}
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
std::list<Token> lexical_analisys(std::string const& source) {
	std::list<Token> res{source};
	separate_comments_and_strings(res);
	split_on_separators(res, " \t\n");
	differenciate_separators(res, ";=>.-()[]{}+,");
	classify_tokens(res);
	clean_tokens(res);
	return res;
}
std::string interpret(std::string const& source) {
	auto lexemes = lexical_analisys(source);
	//throw std::exception(("Unimplemented function was called with source = " + source).c_str());
	return "";
}