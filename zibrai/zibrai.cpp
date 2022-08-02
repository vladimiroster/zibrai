// zibrai.cpp : Zis is barely an interpreter
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "error.h"
#include "scanner.h"

void run(const std::string& source) {
  // Print the tokens
  Scanner scanner(source);
  auto tokens = scanner.scanTokens();

  // Tokenizing w.r.t. space ' '
  for (auto token : tokens) {
    std::cout << token.toString() << std::endl;
  }
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
