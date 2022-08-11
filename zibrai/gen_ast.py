# Code generation script for generating AST data classes
import sys

from typing import List

def define_ast(output_dir: str, base_name: str, types: List[str]):
  def define_type(class_name: str, fields: str):
    field_list = fields.split(", ")

    return f"""struct {class_name} : public {base_name} {{
    {class_name}({fields}) :
      {",".join(f"{name}({name})" for name in map(lambda x: x.split(" ")[1], field_list))}
    {{ }}

    virtual void accept(Visitor& visitor) const override;

    const {(";" + NEWLINE + TAB * 2 + "const ").join([field for field in field_list])};
}};"""

  def define_accept(class_name: str):
    return f"""void {class_name}::accept(Visitor& visitor) const {{
  visitor.visit(this);
}}
"""

  print(f"Outputting to {output_dir}")
  NEWLINE = "\n"
  TAB = "\t"
  type_names = [t.split(':')[0].strip() for t in types]
  with open(f"{output_dir}/{base_name}.h", "w") as f:
    s = f"""#pragma once

class Token;
struct Visitor;
using std::string; // for the ":" tokenizer not to break on std::string

struct {base_name} {{
  virtual void accept(Visitor& visitor) const = 0;
}};


{(NEWLINE*2).join([define_type(t.split(":")[0].strip(), t.split(":")[1].strip()) for t in types])}

struct Visitor {{
  virtual void visit(const {base_name}* expr) = 0;
  {(NEWLINE + TAB).join([f"virtual void visit(const {t}* expr) = 0;" for t in type_names])}
}};

{(NEWLINE*2).join([define_accept(t) for t in type_names])}
    """

    f.write(s)
  

if __name__ == "__main__":
  if len(sys.argv) != 2:
    print("Usage: gen_ast.py <output directory>")
    sys.exit(64)

  output_dir = sys.argv[1]

  define_ast(output_dir, "Expr", [
    "Binary : Expr* left, Token* op, Expr* right",
    "Grouping : Expr* expression",
    "Literal : string value",
    "Unary : Token* op, Expr* right"
  ])