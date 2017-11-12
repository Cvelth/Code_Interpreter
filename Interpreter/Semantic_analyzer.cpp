#include "Interpreter.hpp"
bool semantic_analysis(Syntax const& syntax) {
	return syntax.check_semantics();
}
bool Syntax::check_semantics() const {
	return parse_semantics(graph);
}
bool Syntax::is_string_convertable(std::shared_ptr<Node> node) const {
	if (node->type == TokenType::variable_name || node->type == TokenType::string_literal || node->type == TokenType::int_literal)
		return true;
	else if (node->type == TokenType::binary_operator && node->name == ".")
		return parse_semantics(node);
	else if (node->type == TokenType::bracket && node->name == "()") {
		if (parse_semantics(node, TokenType::variable_name) || parse_semantics(node, TokenType::string_literal) || parse_semantics(node, TokenType::int_literal))
			return true;
		else
			throw std::exception("Unsupported parameters in brackets '()' was used");
	}
	throw std::exception("Unsupported operator was used.");
}
bool Syntax::parse_semantics(std::shared_ptr<Node> syntax, TokenType expectedType) const {
	if (syntax == nullptr)
		return true;
	if (syntax->type == TokenType::semicolon) {
		return parse_semantics(syntax->left) && parse_semantics(syntax->right);
	} else if (syntax->type == TokenType::string_literal || syntax->type == TokenType::int_literal) {
		return true;
	} else if (syntax->type == TokenType::bracket) {
		if (syntax->left != nullptr)
			throw std::exception("Something wrong with the brackets.");
		if (syntax->name == "()") {
			if (syntax->right->type == TokenType::variable_name)
				return true;
			else
				return parse_semantics(syntax->right, expectedType);
		}
	} else if (syntax->type == TokenType::binary_operator) {
		if (syntax->name == ".") {
			if (syntax->left == nullptr || syntax->right == nullptr)
				throw std::exception("operator '.' must have two parameters.");
			return is_string_convertable(syntax->left) && is_string_convertable(syntax->right);
		}
	} else if (syntax->type == TokenType::reserved_word) {
		if (syntax->name == "print") {
			if (syntax->left != nullptr)
				throw std::exception("There must be nothing to the left of print-structure.");
			return is_string_convertable(syntax->right);
		}
	}
	throw std::exception("Unsupported construction was used.");
}