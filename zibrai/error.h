#pragma once

#include <string>

extern bool g_hadError;

void report(int line, std::string loc, std::string msg);
void error(int line, std::string msg);
