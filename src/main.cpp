#include "../include/regmatch.h"
#include <iostream>

int main() {
  Matcher m("(a+)*");

  m.printFSA();

  std::cout << std::boolalpha << std::endl;
  std::cout << m.matchString("aaaa") << '\n';   // true
  std::cout << m.matchString("") << '\n';     // true
  std::cout << m.matchString("ab") << '\n';   // false
  std::cout << m.matchString("aaa") << '\n';  // false
  std::cout << m.matchString("aaaa") << '\n'; // true
}
