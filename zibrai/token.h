#pragma once

#include <string>
#include <sstream>

class Token {
public:
  enum Type {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens
    BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    EoF
    };

    const Type type;
    const std::string lexeme;
    // TODO: remove literal? Pending how the representation looks
    const std::string literal;
    const size_t line;

    Token(Type type, std::string lexeme, std::string literal, size_t line) :
      type(type), lexeme(lexeme), literal(literal), line(line) {}

    std::string toString() {
      std::stringstream retval;
      retval << "Token: " << type << ", " << lexeme << ", " << literal;
      return retval.str();
    }

};
