#include <catch2/catch_test_macros.hpp>
#include "../include/browser.hpp"
#include "../include/utils.hpp"


TEST_CASE( "Basic navigation" ) {

    Browser b{};
    b.goToSite("tests/sites/basic.gmi");
    REQUIRE(b.getCurrentSite()->getBody() == readFileToString("tests/sites/basic.gmi"));
    b.goToSite("basic_2.gmi");
    REQUIRE(b.getCurrentSite()->getBody() == readFileToString("tests/sites/basic_2.gmi"));
}

