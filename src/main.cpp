#include "../include/regmatch.hpp"
#include <iostream>

int main() {
    Matcher m("a+");
    Matcher n("a*a");

    std::cout << std::boolalpha << std::endl;
    std::cout << m.compareRegex(n) << '\n';
}
