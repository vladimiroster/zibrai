#pragma once

#include <string>
#include "expr.h"

class AstPrinter : public Visitor {
public:
  std::string print(const Expr* expr) {
    expr->accept(*this);
    return _sstream.str();
  }

  virtual void visit(const Expr* expr) override {
    _sstream << "Error! Shouldn't have abstract Expression objects." << std::endl;
  }

  virtual void visit(const Binary* expr) override {
    parenthesize(expr->op->lexeme, std::vector<const Expr*>{expr->left, expr->right});

  }
  virtual void visit(const Grouping* expr) override {
    parenthesize("group", std::vector<const Expr*>{expr->expression});
  }
  virtual void visit(const Literal* expr) {
    if (expr->value == "") _sstream << "nil";
    else _sstream << expr->value;
  }
  virtual void visit(const Unary* expr) override {
    parenthesize(expr->op->lexeme, std::vector<const Expr*>{expr->right});
  }

private:
  void parenthesize(std::string name, std::vector<const Expr*> expressions) {
    _sstream << "(" << name;
    for (const auto& e : expressions) {
      _sstream << " ";
      e->accept(*this);
    }
    _sstream << ")";
  }

  std::stringstream _sstream;
};
