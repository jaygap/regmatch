#include "../include/regmatch.hpp"
#include <iostream>

//ISSUE WITH MINIMISATION METHOD
//NO LONGER SEGFAULTS BUT NOW THE LOGIC IS WRONG :3

int main() {
    //Matcher m("(a|b)*c(aa)*");
    //Matcher n("[ab]*c(aa)*");
    Matcher t("aa*b");

    t.printFSA();

    //m.printFSA();

    //n.printFSA();

    std::cout << std::boolalpha << std::endl;
    //std::cout << m.compareRegex(n) << '\n';
}
