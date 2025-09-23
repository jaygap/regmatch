#include "../include/regmatch.hpp"
#include <iostream>

int main() {
  Matcher m("[a-Z]+");

  m.printFSA();

  std::cout << std::boolalpha << std::endl;
  std::cout << m.matchString("abcdefgh") << '\n';
  std::cout << m.matchString("ABCDEFGH") << '\n';
  std::cout << m.matchString("hgfedcba") << '\n';
  std::cout << m.matchString("HGFEDCBA") << '\n';
  std::cout << m.matchString("aBcDeFgH") << '\n';
  std::cout << m.matchString("hGfEdCbA") << '\n';
}
