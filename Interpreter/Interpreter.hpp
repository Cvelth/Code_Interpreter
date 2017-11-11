#pragma once
#include "dll.hpp"
#include <list>
#include "Token.hpp"
DLL std::list<Token> lexical_analisys(std::string const& source);
DLL std::string interpret(std::string const& source);