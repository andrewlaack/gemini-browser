#include "../include/browser.hpp"
#include "../include/site.hpp"
#include "../include/link.hpp"
#include <iostream>

int main() {
    Browser b{};
    b.goToSite("tests/sites/basic.gmi");
    b.goToSite("basic_2.gmi");
}
