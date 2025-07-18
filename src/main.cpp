#include "../include/regmatch.hpp"
#include <iostream>

int main() {
  Matcher m("a|b|c");

  m.printFSA();

  std::cout << std::boolalpha << std::endl;
  std::cout << m.matchString("a") << '\n';
  std::cout << m.matchString("b") << '\n';
  std::cout << m.matchString("c") << '\n';
  std::cout << m.matchString("ab") << '\n';
  std::cout << m.matchString("ac") << '\n';
  std::cout << m.matchString("bc") << '\n';
}
