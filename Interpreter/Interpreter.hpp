#pragma once
#include "dll.hpp"
#include <list>
#include "Token.hpp"
#include "Syntax.hpp"
DLL std::list<Token> lexical_analysis(std::string const& source);
DLL Syntax syntax_analysis(std::list<Token> const& source);
DLL bool semantic_analysis(Syntax const& syntax);
DLL std::string code_recreation(Syntax const& syntax, bool semantic_result, bool print_comments = true);
DLL std::string interpret(std::string const& source);