#pragma once

#include "expr.h"

#include <iostream>
#include <iomanip>
#include <stack>

union value_t {
  double d;
  char* s; // TODO: fix strings
  bool b;
};

enum type_t {
  DOUBLE,
  BOOL,
  STRING,
};

struct LoxValue {
  value_t val;
  type_t type;
};

// TODO: find the way to do a tagged union with std::string?
//struct LoxValue {
//  union {
//    double d;
//    bool b;
//    std::string s;
//  } val;
//  type_t type;
//
//  LoxValue() : type(DOUBLE) {
//    val.d = 0;
//  }
//
//  LoxValue(const LoxValue& other) : type(other.type) {
//    switch (type) {
//      case DOUBLE: val.d = other.val.d; break;
//      case BOOL: val.d = other.val.d; break;
//      case STRING: new(&val.s)(other.val.s); break;
//    }
//  }
//
//  ~LoxValue() {
//    if (type == STRING) {
//      val.s.~string();
//    }
//  }
//};


class Interpreter : public Visitor {
public:
  void interpret(const Expr* expr) {
    try {
      evaluate(expr);
      std::cout << stringify(_stack.top()) << std::endl;
      _stack.pop();
    }
    catch (const RuntimeError& e) {
      runtimeError(e);
    }
  }

  void evaluate(const Expr* expr) {
    expr->accept(*this);
    if (_stack.size() != 1) {
      throw RuntimeError(nullptr, "Interpreter value stack is corrupt"); // TODO: add the stack to the exception
    }
  }

  virtual void visit(const Expr* expr) override {
    // NOP
  }

  virtual void visit(const Binary* expr) override {
    evaluate(expr->left);
    auto left = _stack.top();
    _stack.pop();
    evaluate(expr->right);
    auto right = _stack.top();
    _stack.pop();

    LoxValue retval;
    switch (expr->op->type) {
      case Token::MINUS:
        checkNumberOperands(expr->op, left, right);
        retval.val.d = left.val.d - right.val.d;
        retval.type = DOUBLE;
        break;
      case Token::SLASH:
        checkNumberOperands(expr->op, left, right);
        retval.val.d = left.val.d / right.val.d;
        retval.type = DOUBLE;
        break;
      case Token::STAR:
        checkNumberOperands(expr->op, left, right);
        retval.val.d = left.val.d * right.val.d;
        retval.type = DOUBLE;
        break;
      case Token::PLUS:
        if ((left.type == DOUBLE) && (right.type == DOUBLE)) {
          retval.val.d = left.val.d + right.val.d;
          retval.type = DOUBLE;
        } else if ((left.type == STRING) && (right.type == STRING)) {
          retval.val.s = new char[strlen(left.val.s) + strlen(right.val.s) + 1];
          strncpy(retval.val.s, left.val.s, strlen(left.val.s));
          strncpy(retval.val.s + strlen(left.val.s), right.val.s, strlen(right.val.s));
          retval.type = STRING;
        } else throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");

        break;
      case Token::GREATER:
        checkNumberOperands(expr->op, left, right);
        retval.val.b = left.val.d > right.val.d;
        retval.type = BOOL;
        break;
      case Token::GREATER_EQUAL:
        checkNumberOperands(expr->op, left, right);
        retval.val.b = left.val.d >= right.val.d;
        retval.type = BOOL;
        break;
      case Token::LESS:
        checkNumberOperands(expr->op, left, right);
        retval.val.b = left.val.d < right.val.d;
        retval.type = BOOL;
        break;
      case Token::LESS_EQUAL:
        checkNumberOperands(expr->op, left, right);
        retval.val.b = left.val.d <= right.val.d;
        retval.type = BOOL;
        break;
      case Token::BANG_EQUAL:
        retval.val.b = !isEqual(left, right);
        retval.type = BOOL;
        break;
      case Token::EQUAL_EQUAL:
        retval.val.b = isEqual(left, right);
        retval.type = BOOL;
        break;
    }
    _stack.push(std::move(retval));
  }

  virtual void visit(const Grouping* expr) override {
    evaluate(expr->expression);
  }

  virtual void visit(const Literal* expr) override {
    LoxValue retval;
    // First see if it's a number

    try {
      double d = std::stod(expr->value);
      retval.val.d = d;
      retval.type = DOUBLE;
    }
    catch (const std::invalid_argument&) {
      // not a double
      if (expr->value == "true") {
        retval.val.b = true;
        retval.type = BOOL;
      }
      else if (expr->value == "false") {
        retval.val.b = false;
        retval.type = BOOL;
      }
      else {
        // TODO: stop leaking this
        retval.val.s = new char[expr->value.length() + 1];
        strncpy(retval.val.s, expr->value.c_str(), expr->value.length());
        retval.type = STRING;
      }
    }
    
    // TODO: I'm pretty sure I'm forgetting NIL, but it's pretty late
    // and I'll handle it next week
    _stack.push(std::move(retval));
  }

  virtual void visit(const Unary* expr) override {
    evaluate(expr->right);
    auto val = _stack.top();
    _stack.pop();

    switch (expr->op->type) {
      case Token::MINUS:
        checkNumberOperand(expr->op, val);
        val.val.d = -val.val.d;
        _stack.push(std::move(val));
        break;
      case Token::BANG:
        val.val.b = !isTruthy(val);
        _stack.push(val);
    }
  }
private:
  std::string stringify(const LoxValue& val) {
    // if object == null return "nil"
    std::stringstream ss;
    switch (val.type) {
    case DOUBLE:
      ss << std::setprecision(5) << val.val.d;
      break;
    case BOOL:
      ss << std::boolalpha << val.val.b;
      break;
    case STRING:
      ss << val.val.s;
      break;
    }

    return ss.str();
  }

  bool isTruthy(const LoxValue& val) {
    // TODO: if null return false
    if (val.type == BOOL) {
      return val.val.b;
    }
    return true;

  }

  bool isEqual(const LoxValue& left, const LoxValue& right) {
    if (left.type != right.type) return false;

    switch (left.type) {
      case BOOL:
        return left.val.b == right.val.b;
      case DOUBLE:
        return left.val.d == right.val.d;
      case STRING:
        return left.val.s == right.val.s;
    }
    return false;
  }

  void checkNumberOperand(const Token* op, const LoxValue& operand) {
    if (operand.type == DOUBLE) return;
    throw RuntimeError(op, "Operand must be number.");
  }

  void checkNumberOperands(const Token* op, const LoxValue& left, const LoxValue& right) {
    // TODO: that sounds like could be checkNumberOperand called twice
    if ((left.type == DOUBLE) && (right.type == DOUBLE)) return;
    throw RuntimeError(op, "Operands must be numbers.");
  }

  std::stack<LoxValue> _stack;
};