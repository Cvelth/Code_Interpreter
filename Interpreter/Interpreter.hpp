#pragma once
#include "Lexeme.hpp"
#include <string>
#include <list>
DLL std::list<Lexeme> lexical_analisys(std::string const& source);
DLL std::string interpret(std::string const& source);