#include "../include/regmatch.hpp"
#include <iostream>

int main() {
  Matcher m("https://([a-Z0-9]+\\.)?[a-Z0-9]+(\\.[a-Z0-9]+)+");

  m.printFSA();

  std::cout << std::boolalpha << std::endl;
  std::cout << m.matchString("https://google.com") << '\n';
  std::cout << m.matchString("https://www.qmul.ac.uk") << '\n';
  std::cout << m.matchString("https://www.linkedin.com") << '\n';
  std::cout << m.matchString("https://www.qmplus.org") << '\n';
}
