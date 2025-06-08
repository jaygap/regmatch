#include "../include/regmatch.hpp"
#include <iostream>

int main() {
  Matcher m("ab|cd|ef");

  m.printFSA();

  std::cout << std::boolalpha << std::endl;
  std::cout << m.matchString("ab") << '\n';
  std::cout << m.matchString("") << '\n';
  std::cout << m.matchString("cccc") << '\n';
  std::cout << m.matchString("abcabababc") << '\n';
  std::cout << m.matchString("ace") << '\n';
}
