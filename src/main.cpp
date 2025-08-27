#include "../include/regmatch.hpp"
#include <iostream>

int main() {
  Matcher m("(a|b)+");

  m.printFSA();

  std::cout << std::boolalpha << std::endl;
  std::cout << m.matchString("aaaaa") << '\n';
  std::cout << m.matchString("bbbbb") << '\n';
  std::cout << m.matchString("ccccc") << '\n';
  std::cout << m.matchString("aabbcc") << '\n';
  std::cout << m.matchString("abbccc") << '\n';
  std::cout << m.matchString("ac") << '\n';
  std::cout << m.matchString("bc") << '\n';
  std::cout << m.matchString("") << '\n';
}
