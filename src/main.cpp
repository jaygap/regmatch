#include "../include/regmatch.hpp"
#include <iostream>

int main() {
    Matcher m("(abc)+");
    Matcher n("[ab]+");

    m.printFSA();

    std::cout << std::boolalpha << std::endl;
    std::cout << m.compareRegex(n) << '\n';
}
