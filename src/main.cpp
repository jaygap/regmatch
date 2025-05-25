#include <iostream>
#include "regmatch.h"

int main(){
    Matcher m("(aa)*a?");

    m.printFSA();
    FSA f = m.convertToMinDFA((m.getFSA()));

    f.printFSA();

    std::cout << std::boolalpha << std::endl;
    std::cout << m.matchString("aa") << '\n';   // true
    std::cout << m.matchString("") << '\n';     // true
    std::cout << m.matchString("ab") << '\n';   // false
    std::cout << m.matchString("aaa") << '\n';  // false
    std::cout << m.matchString("aaaa") << '\n'; // true
}