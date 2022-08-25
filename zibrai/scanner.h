#pragma once

#include <string>
#include <list>
#include <unordered_map>

#include "token.h"
#include "error.h"

class Scanner {
public:
  Scanner(std::string source) : _source(source) {}

  std::vector<Token*> scanTokens() {
    while (!isAtEnd()) {
      _start = _current;
      scanToken();
    }

    _tokens.emplace_back(new Token(Token::Type::EoF, "", "", _line));
    return _tokens;
  }

private:
  void scanToken() {
    char c = advance();
    switch (c) {
      case '(': addToken(Token::Type::LEFT_PAREN); break;
      case ')': addToken(Token::Type::RIGHT_PAREN); break;
      case '{': addToken(Token::Type::LEFT_BRACE); break;
      case '}': addToken(Token::Type::RIGHT_BRACE); break;
      case ',': addToken(Token::Type::COMMA); break;
      case '.': addToken(Token::Type::DOT); break;
      case '-': addToken(Token::Type::MINUS); break;
      case '+': addToken(Token::Type::PLUS); break;
      case ';': addToken(Token::Type::SEMICOLON); break;
      case '*': addToken(Token::Type::STAR); break;
      case '!':
        addToken(match('=') ? Token::Type::BANG_EQUAL : Token::Type::BANG);
        break;
      case '=':
        addToken(match('=') ? Token::Type::EQUAL_EQUAL : Token::Type::EQUAL);
        break;
      case '<':
        addToken(match('=') ? Token::Type::LESS_EQUAL : Token::Type::LESS);
        break;
      case '>':
        addToken(match('=') ? Token::Type::GREATER_EQUAL : Token::Type::GREATER);
        break;
      case '/':
        if (match('/')) {
          while (peek() != '\n' && !isAtEnd()) advance();
        }
        else {
          addToken(Token::Type::SLASH);
        }
        break;
      case ' ':
      case '\r':
      case '\t':
        // Ignore whitespaces
        break;
      case '\n':
        _line++;
        break;
      case '"': string(); break;

      default:
        if (isDigit(c)) {
          number();
        } else if (isAlpha(c)) {
          identifier();
        } else {
          error(_line, "Unexpected character.");
        }
        break;
    }
  }

  bool isAtEnd() {
    return _current >= _source.length();
  }

  char advance() {
    return _source[_current++];
  }

  void addToken(Token::Type type, std::string literal = "") {
    std::string text = _source.substr(_start, _current - _start);
    _tokens.emplace_back(new Token(type, text, literal, _line));
  }

  bool match(char expected) {
    if (isAtEnd()) return false;
    if (_source[_current] != expected) return false;

    _current++;
    return true;
  }

  char peek() {
    if (isAtEnd()) return '\0';
    return _source[_current];
  }

  char peekNext() {
    if (_current + 1 >= _source.length()) return '\0';
    return _source[_current + 1];
  }

  void string() {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n') _line++;
      advance();
    }

    if (isAtEnd()) {
      error(_line, "Unterminated string.");
      return;
    }

    advance(); // The closing '"'

    // Trim the surrounding quotes.
    std::string value = _source.substr(_start + 1, _current - _start - 1);
    addToken(Token::Type::STRING, value);
  }

  bool isDigit(char c) {
    return c >= '0' && c <= '9';
  }

  void number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part
    if (peek() == '.' && isDigit(peekNext())) {
      // consume the "."
      advance();

      while (isDigit(peek())) advance();
    }

    addToken(Token::Type::NUMBER, _source.substr(_start, _current - _start));
  }

  void identifier() {
    while (isAlphaNumeric(peek())) advance();

    auto text = _source.substr(_start, _current - _start);
    auto it = _keywords.find(text);
    Token::Type type;
    if (it != _keywords.end()) {
      type = it->second;
    }
    else {
      type = Token::Type::IDENTIFIER;
    }

    addToken(type);
  }
  bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
  }

  bool isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
  }

  const std::string _source;
  std::vector<Token*> _tokens;
  size_t _start = 0;
  size_t _current = 0;
  size_t _line = 1;

  std::unordered_map<std::string, Token::Type> _keywords = {
    {"and", Token::Type::AND},
    {"class", Token::Type::CLASS},
    {"else", Token::Type::ELSE},
    {"false", Token::Type::FALSE},
    {"for", Token::Type::FOR},
    {"fun", Token::Type::FUN},
    {"if", Token::Type::IF},
    {"nil", Token::Type::NIL},
    {"or", Token::Type::OR},
    {"print", Token::Type::PRINT},
    {"return", Token::Type::RETURN},
    {"super", Token::Type::SUPER},
    {"this", Token::Type::THIS},
    {"true", Token::Type::TRUE},
    {"var", Token::Type::VAR},
    {"while", Token::Type::WHILE}
  };
};