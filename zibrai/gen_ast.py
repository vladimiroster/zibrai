# Code generation script for generating AST data classes
import sys

from typing import List

def define_ast(output_dir: str, base_name: str, types: List[str]):
  def define_type(class_name: str, fields: str):
    field_list = fields.split(", ")

    return f"""class {class_name} : public {base_name} {{
    {class_name}({fields}) {{
        {(NEWLINE + TAB * 2).join(f"this->{name} = {name};" for name in map(lambda x: x.split(" ")[1], field_list))}
    }}

    const {(";" + NEWLINE + TAB + "const ").join([field for field in field_list])};
}};"""

  print(f"Outputting to {output_dir}")
  NEWLINE = "\n"
  TAB = "\t"
  with open(f"{output_dir}/{base_name}.h", "w") as f:
    s = f"""#pragma once

class Token;

class {base_name} {{}};

{(NEWLINE*2).join([define_type(t.split(":")[0].strip(), t.split(":")[1].strip()) for t in types])}
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
    "Literal : void* value",
    "Unary : Token* op, Expr* right"
  ])