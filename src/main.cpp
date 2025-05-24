#include <iostream>
#include "regmatch.h"

int main(){
    Matcher m("(aa)*");

    m.printFSA();
    m.convertToMinDFA(m.getFSA()).printFSA();

    std::cout << std::boolalpha << std::endl;
    std::cout << m.matchString("aa") << '\n';   // true
    std::cout << m.matchString("") << '\n';     // true
    std::cout << m.matchString("ab") << '\n';   // false
    std::cout << m.matchString("aaa") << '\n';  // false
    std::cout << m.matchString("aaaa") << '\n'; // true
}