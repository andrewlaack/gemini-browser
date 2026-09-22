#include "../include/browser.hpp"
#include "../include/site.hpp"
#include "../include/link.hpp"
#include <iostream>

int main() {
    Browser b{};
    b.goToSite("gemini://laack.co");
}
