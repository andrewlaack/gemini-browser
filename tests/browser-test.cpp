#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <utility>
#include "../include/browser.hpp"
#include "../include/utils.hpp"


TEST_CASE( "Basic navigation" ) {

    Browser b{};
    b.goToSite("file:///home/andrew/gitRepos/gemini-browser/tests/sites/basic.gmi", true);
    REQUIRE(b.getCurrentSite()->getBody() == readFileToString("tests/sites/basic.gmi"));
    b.goToSite("file:///home/andrew/gitRepos/gemini-browser/tests/sites/basic_2.gmi", true);
    REQUIRE(b.getCurrentSite()->getBody() == readFileToString("tests/sites/basic_2.gmi"));
}


TEST_CASE( "Local filesystem relative navigation" ) {

    Browser b{};
    b.goToSite("file:///home/andrew/gitRepos/gemini-browser/tests/sites/basic.gmi", true);
    REQUIRE(b.getCurrentSite()->getBody() == readFileToString("tests/sites/basic.gmi"));
    b.goToSite("basic_2.gmi", true);
    REQUIRE(b.getCurrentSite()->getBody() == readFileToString("tests/sites/basic_2.gmi"));
}


TEST_CASE("Test trivial line breaking") {
    std::vector<std::pair<std::string, int>> strLs {};
    for(int i = 0; i < 10; ++i) {
        std::pair<std::string, int> current {"this is a simple test line", 10};
        strLs.push_back(current);
    }
    auto result = breakLines(strLs, 10);
    REQUIRE(result.size() ==  30);
}

static std::string charset = "abc defghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

TEST_CASE("Test width invariant") {
    
    std::vector<std::pair<std::string, int>> strLs {};

    srand(std::time(NULL));

    for(int i = 0; i < 10000; ++i) {
        std::string strRnd;
        int ub = rand() % 10000;
        for(int x = 0; x < ub; ++x) {
            strRnd += charset[rand() % charset.length()];
        }
        std::pair<std::string, int> current {strRnd, 10};
        strLs.push_back(current);
    }

    auto result = breakLines(strLs, 80);

    for(auto& res: result) {
        REQUIRE(res.first.size() <= 80);
    }

}

TEST_CASE("Test lots of spaces") {
    
    std::vector<std::pair<std::string, int>> strLs {};

    srand(std::time(NULL));

    for(int i = 0; i < 10; ++i) {
        std::string strRnd;
        int ub = rand() % 10000;
        for(int x = 0; x < ub; ++x) {
            strRnd += ' ';
        }
        std::pair<std::string, int> current {strRnd, 10};
        strLs.push_back(current);
    }

    auto result = breakLines(strLs, 80);

    for(auto& res: result) {
        REQUIRE(res.first.size() <= 80);
    }

}
