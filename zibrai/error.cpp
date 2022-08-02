#include "error.h"

#include <iostream>

bool g_hadError = false;

void report(int line, std::string loc, std::string msg) {
  std::cerr << "[line " << line << "] Error" << loc << ": " << msg;
  g_hadError = true;
}

void error(int line, std::string msg) {
  report(line, "", msg);
}
