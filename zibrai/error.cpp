#include "error.h"

#include <iostream>

#include "token.h"

bool g_hadError = false;
bool g_hadRuntimeError = false;

void report(int line, std::string loc, std::string msg) {
  std::cerr << "[line " << line << "] Error" << loc << ": " << msg << std::endl;
  g_hadError = true;
}

void error(int line, std::string msg) {
  report(line, "", msg);
}

void error(Token* token, std::string msg) {
  if (token->type == Token::EoF) {
    report(token->line, " at end", msg);
  }
  else {
    report(token->line, " at '" + token->lexeme + "'", msg);
  }
}

void runtimeError(const RuntimeError& err) {
  std::cerr << err.msg() << "\n[line " << err.op()->line << "]\n";
  g_hadRuntimeError = true;
}