// zibrai.cpp : Zis is barely an interpreter
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "error.h"
#include "scanner.h"
#include "astprinter.h"
#include "parser.h"

// Auto generated
#include "Expr.h"

// TODO: smart pointers

void run(const std::string& source) {
  // Print the tokens
  Scanner scanner(source);
  auto tokens = scanner.scanTokens();

  Parser parser(tokens);
  Expr* expression = parser.parse();

  if (g_hadError) return;

  std::cout << AstPrinter().print(expression) << std::endl;
}

void runPrompt() {
  std::string input;
  while (true) {
    std::cout << ">>> ";
    std::getline(std::cin, input);
    if (input == "quit") {
      break;
    }
    run(input);
    g_hadError = false;
  }
}

void runFile(const std::string& path) {
  std::ifstream ifs(path);
  std::stringstream buffer;
  buffer << ifs.rdbuf();
  run(buffer.str());
  if (g_hadError) exit(65);
}

int main(int argc, char* argv[])
{
  if (argc > 1) {
    std::cerr << "Usage: zibrai [script]" << std::endl;
    exit(64);
  }
  else if (argc == 2) {
    runFile(argv[1]);
  }
  else {
    runPrompt();
  }
    
}
