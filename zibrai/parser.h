#pragma once

#include <vector>

#include "token.h"
#include "expr.h"
#include "error.h"

/*
Parser grammar:
expression -> equality
equality   -> comparison ( ( "!=" | "==" ) comparison )*
comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*
term       -> factor ( ( "-" | "+" ) factor )*
factor     -> unary ( ( "/" | "*" ) unary )*
unary      -> ( "!" | "-") unary | primary
primary    -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
*/
class Parser {
public:

  class ParseError {};

  Parser(std::vector<Token*> tokens) : _tokens(std::move(tokens)) {}

  Expr* parse() {
    try {
      return expression();
    }
    catch (ParseError) {
      return nullptr;
    }
  }

private:
  Expr* expression() {
    return equality();
  }

  // TODO: template this
  Expr* equality() {
    Expr* expr = comparison();
    while (match({Token::BANG_EQUAL, Token::EQUAL_EQUAL})) {
      Token* op = previous();
      Expr* right = comparison();
      expr = new Binary(expr, op, right);
    }

    return expr;
  }

  Expr* comparison() {
    Expr* expr = term();
    while (match({ Token::GREATER, Token::GREATER_EQUAL, Token::LESS, Token::LESS_EQUAL })) {
      Token* op = previous();
      Expr* right = term();
      expr = new Binary(expr, op, right);
    }

    return expr;
  }

  Expr* term() {
    Expr* expr = factor();
    while (match({ Token::MINUS, Token::PLUS })) {
      Token* op = previous();
      Expr* right = factor();
      expr = new Binary(expr, op, right);
    }

    return expr;
  }

  Expr* factor() {
    Expr* expr = unary();
    while (match({ Token::SLASH, Token::STAR })) {
      Token* op = previous();
      Expr* right = unary();
      expr = new Binary(expr, op, right);
    }

    return expr;
  }

  Expr* unary() {
    if (match({ Token::BANG, Token::MINUS })) {
      Token* op = previous();
      Expr* right = unary();
      return new Unary(op, right);
    }

    return primary();
  }

  Expr* primary() {
    if (match({Token::FALSE})) return new Literal("False");
    if (match({Token::TRUE })) return new Literal("True");
    if (match({Token::NIL })) return new Literal("Nil");

    if (match({Token::NUMBER, Token::STRING})) return new Literal(previous()->literal);

    if (match({ Token::LEFT_PAREN })) {
      Expr* expr = expression();
      consume(Token::RIGHT_PAREN, "Expect ')' after expression.");
      return new Grouping(expr);
    }

    throw error(peek(), "Expect expression.");

  }

  bool match(std::vector<Token::Type> types) {
    for (auto t : types) {
      if (check(t)) {
        advance();
        return true;
      }
    }
    return false;
  }

  bool check(Token::Type type) {
    if (isAtEnd()) return false;
    return peek()->type == type;
  }

  Token* advance() {
    if (!isAtEnd()) _current++;
    return previous();
  }

  bool isAtEnd() {
    return peek()->type == Token::EoF;
  }

  Token* peek() {
    return _tokens[_current];
  }

  Token* previous() {
    return _tokens[_current - 1];
  }

  Token* consume(Token::Type type, std::string msg) {
    if (check(type)) return advance();

    throw error(peek(), msg);
  }

  ParseError error(Token* token, std::string msg) {
    ::error(token, msg);
    return ParseError();
  }

  void synchronize() {
    advance();
    while (!isAtEnd()) {
      if (previous()->type == Token::SEMICOLON) return;

      switch (peek()->type) {
        case Token::CLASS: case Token::FOR: case Token::FUN: case Token::IF:
        case Token::PRINT: case Token::RETURN: case Token::VAR: case Token::WHILE:
          return;
      }

      advance();
    }
  }
  const std::vector<Token*> _tokens;
  size_t _current = 0;
};