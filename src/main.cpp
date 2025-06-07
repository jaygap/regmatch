#include "../include/regmatch.h"
#include <iostream>

int main() {
  Matcher m("((ab)*|c)+");

  m.printFSA();

  std::cout << std::boolalpha << std::endl;
  std::cout << m.matchString("ab") << '\n';   // true
  std::cout << m.matchString("") << '\n';     // true
  std::cout << m.matchString("cccc") << '\n';   // false
  std::cout << m.matchString("abcabababc") << '\n';  // false
  std::cout << m.matchString("ace") << '\n'; // true
}
