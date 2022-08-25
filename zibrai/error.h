#pragma once

#include <string>

#include "token.h"

extern bool g_hadError;

void report(int line, std::string loc, std::string msg);
void error(int line, std::string msg);
void error(Token* token, std::string msg);
