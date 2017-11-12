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
	else if (node->type == TokenType::binary_operator && (node->name == "." || node->name == "->"))
		return parse_semantics(node);
	else if (node->type == TokenType::bracket && node->name == "()") {
		if (parse_semantics(node, TokenType::variable_name) || parse_semantics(node, TokenType::string_literal) || parse_semantics(node, TokenType::int_literal))
			return true;
		else
			throw std::exception("Unsupported parameters in brackets '()' were used");
	}
	throw std::exception("Unsupported operator was used.");
}
bool Syntax::is_rvalue(std::shared_ptr<Node> node) const {
	if (node->type == TokenType::variable_name || node->type == TokenType::string_literal || node->type == TokenType::int_literal)
		return true;
	else if (node->type == TokenType::reserved_word && node->name == "shift")
		return true;
	else if (node->type == TokenType::binary_operator && (node->name == "." || node->name == "->"))
		return parse_semantics(node);
	else if (node->type == TokenType::bracket) {
		if (node->name == "()")
			if (parse_semantics(node, TokenType::variable_name) || parse_semantics(node, TokenType::string_literal) || parse_semantics(node, TokenType::int_literal))
				return true;
			else
				throw std::exception("Unsupported parameters in '()' brackets were used");
		else if (node->name == "{}")
			if (node->right->type == TokenType::variable_name || node->right->type == TokenType::list_name || node->right->type == TokenType::hash_name || node->right->type == TokenType::type_name)
				return true;
			else
				throw std::exception("Unsupported parameters in '{}' brackets were used");
	}
	throw std::exception("Unsupported operator was used.");
}
bool Syntax::is_function_arguments(std::shared_ptr<Node> node) const {
	if (node->type == TokenType::binary_operator) {
		if (node->name == "=>" && node->left->type == TokenType::type_name && node->right->type == TokenType::string_literal)
			return true;
		else if (node->name == ",")
			return is_function_arguments(node->left) && is_function_arguments(node->right);
	}
	return false;
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
		} else if(syntax->name == "{}") {
			if (expectedType == TokenType::unknown)
				return parse_semantics(syntax->right);
			else {
				if (expectedType == TokenType::variable_name || expectedType == TokenType::list_name || expectedType == TokenType::hash_name)
					if (syntax->right->type == expectedType)
						return true;
			}
		}
		throw std::exception("Unsupported brackets were used.");
	} else if (syntax->type == TokenType::binary_operator) {
		if (syntax->name == ".") {
			if (syntax->left == nullptr || syntax->right == nullptr)
				throw std::exception("operator '.' must have two parameters.");
			return is_string_convertable(syntax->left) && is_string_convertable(syntax->right);
		} else if (syntax->name == "->") {
			if (syntax->right->type == TokenType::function) {
				if (syntax->left->type != TokenType::type_name)
					throw std::exception(("Typename was expected as left-hand argument of operator-> for static methods, but " + syntax->left->name + " was found instead").c_str());
				return parse_semantics(syntax->right);
			} else {
				if (syntax->left->type != TokenType::variable_name)
					throw std::exception(("Variable was expected as left-hand argument of operator-> for method calls, but " + syntax->left->name + " was found instead").c_str());
				if (syntax->right->type != TokenType::type_name && !(syntax->right->type == TokenType::bracket && syntax->right->name == "{}" && syntax->right->right->type == TokenType::type_name))
					throw std::exception(("'Typename' or '{typename}' was expected as right-hand operator of operator->, but " + syntax->right->name + " was found instead").c_str());
			} 
		} else if(syntax->name == "+=") {
			if (syntax->left->type != TokenType::variable_name)
				throw std::exception(("Variable was expected as left-hand argument of operator+=, but " + syntax->left->name + " was found instead").c_str());
			if (!is_rvalue(syntax->right))
				throw std::exception(("rvalue was expected as right-hand argument of operator+=, but " + syntax->right->name + " was found instead").c_str());
		}
		return true;
	} else if (syntax->type == TokenType::reserved_word) {
		if (syntax->name == "print") {
			if (syntax->left != nullptr)
				throw std::exception(("There must be nothing to the left of print-structure, but " + syntax->left->name + " was found").c_str());
			return is_string_convertable(syntax->right);
		} else if (syntax->name == "package") {
			if (syntax->left != nullptr)
				throw std::exception(("There must be nothing to the left of package-structure, but " + syntax->left->name + " was found").c_str());
			if (syntax->right->type != TokenType::type_name)
				throw std::exception(("Typename was expected in package-structure, but " + syntax->right->name + " was found instead").c_str());
			return true;
		} else if (syntax->name == "return") {
			if (syntax->left != nullptr)
				throw std::exception(("There must be nothing to the left of return-structure, but " + syntax->left->name + " was found").c_str());
			if (!is_rvalue(syntax->right))
				throw std::exception(("rvalue was expected in return-structure, but " + syntax->right->name + " was found instead").c_str());
			return true;
		} else if (syntax->name == "sub") {
			if (syntax->left->type != TokenType::type_name)
				throw std::exception(("Typename was expected for subroutine initialization, but " + syntax->left->name + " was found instead").c_str());
			if (syntax->right->type != TokenType::bracket || syntax->right->name != "{}")
				throw std::exception(("Body of the subroutine was expected, but " + syntax->right->name + " was found instead").c_str());
			return parse_semantics(syntax->right);
		} else if (syntax->name == "my") {
			if (syntax->left->type != TokenType::variable_name)
				throw std::exception(("Variable was expected for 'my'-structure, but " + syntax->left->name + " was found instead").c_str());
			if (!is_rvalue(syntax->right))
				throw std::exception(("Body of the subroutine was expected, but " + syntax->right->name + " was found instead").c_str());
			return true;
		} else if (syntax->name == "our") {
			if (syntax->left->type != TokenType::list_name)
				throw std::exception(("List was expected for 'our'-structure, but " + syntax->left->name + " was found instead").c_str());
			if (syntax->right->type != TokenType::type_name)
				throw std::exception(("Typename was expected for 'our'-structure, but " + syntax->right->name + " was found instead").c_str());
			return true;
		} else if (syntax->name == "bless") {
			if (syntax->left->type != TokenType::variable_name)
				throw std::exception(("Variable was expected for 'bless'-structure, but " + syntax->left->name + " was found instead").c_str());
			if (syntax->right->type != TokenType::variable_name)
				throw std::exception(("Variable was expected for 'bless'-structure, but " + syntax->right->name + " was found instead").c_str());
			return true;
		} else if (syntax->name == "use") {
			if (syntax->left->type != TokenType::type_name)
				throw std::exception(("Typename was expected for 'use'-structure, but " + syntax->left->name + " was found instead").c_str());
			if (syntax->left->name == "lib") {
				if (syntax->right->type != TokenType::string_literal)
					throw std::exception(("String literal was expected for 'use lib'-structure, but " + syntax->right->name + " was found instead").c_str());
			} else if (syntax->right != nullptr)
				throw std::exception(("Unexpected parameter was found in 'lib'-structure: " + syntax->right->name).c_str());
			return true;
		} else if (syntax->name == "foreach") {
			if (syntax->left->type != TokenType::variable_name)
				throw std::exception(("Variable was expected for 'foreach'-structure, but " + syntax->left->name + " was found instead").c_str());
			if (syntax->right->type != TokenType::reserved_word || syntax->right->name != "foreach_inner")
				throw std::exception("'foreach'-structure is corrupted.");
			if (syntax->right->left->type != TokenType::bracket || syntax->right->left->name != "()" || syntax->right->left->right->type != TokenType::list_name)
				throw std::exception("Unsupported container was speccified in 'foreach'-structure");
			if (syntax->right->right->type != TokenType::bracket || syntax->right->right->name != "{}")
				throw std::exception("Something's wrong with 'foreach'-structure body.");
			return true;
		}
	} else if (syntax->type == TokenType::function) {
		if (syntax->left->type != TokenType::type_name)
			throw std::exception(("Something wrong with the function: " + syntax->name).c_str());
		if (!is_function_arguments(syntax->right))
			throw std::exception(("Something wrong with the arguments of function: " + syntax->name).c_str());
		return true;
	}
	throw std::exception("Unsupported construction was used.");
}