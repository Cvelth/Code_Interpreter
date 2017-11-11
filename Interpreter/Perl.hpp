#pragma once
namespace lang {
	const char comment_start = '#';
	const char comment_end = '\n';
	const char string_literal = '"';
	const char chars_literal = '\'';

	const char variable = '$';
	const char list = '@';
	const char hash = '%';
}
#include <set>
namespace lang {
	const std::set<std::string> reserved_words = {
		"print", "use", "my",
		"package", "sub", "bless", "shift", "return"
		"our", "foreach", "new"
	};
	const std::set<std::string> operators_1 = {
		".", "=", ","
	};
	const std::set<std::string> operators_2 = {
		"=>", "->"
	};
}