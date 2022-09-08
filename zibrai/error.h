#pragma once

#include <string>

#include "token.h"

extern bool g_hadError;
extern bool g_hadRuntimeError;

class RuntimeError {
public:
  RuntimeError(const Token* op, std::string msg) :
    _op(op), _msg(msg) {}

  const Token* op() const {
    return _op;
  }

  std::string msg() const {
    return _msg;
  }

private:
  const Token* _op;
  std::string _msg;
};


void report(int line, std::string loc, std::string msg);
void error(int line, std::string msg);
void error(Token* token, std::string msg);
void runtimeError(const RuntimeError& err);
