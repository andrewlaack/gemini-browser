#include <catch2/catch_test_macros.hpp>
#include "../include/utils.hpp"
#include <filesystem>

#include "../include/gemini-client.hpp"
#include "../include/link.hpp"

TEST_CASE("Link constructors") {

    auto l2  = Link{"=> basic_2.gmi Other Page!", uri{"file://tests/sites/basic.gmi"}};

    REQUIRE("file://tests/sites/basic_2.gmi" == l2.getLinkDestination().to_string());
    REQUIRE("Other Page!" == l2.getLinkText());
}

// this mostly exists in case utils.cpp is busted in some very obvious way.
TEST_CASE("Local Sanity Client") {
    auto client = GeminiClient {};
    auto ln = Link{"=> tests/sites/basic_2.gmi"};
    Site* s = client.fetchSite(ln);

    std::string expectedSite = 
        "# Hello World 2\n"
        "\n"
        "This is basic_2.gmi!\n"
        "\n"
        "=> basic.gmi Back to the basics!\n";

    REQUIRE(s->getBody() == expectedSite);
    delete s;

}

TEST_CASE("Match on all .gmi files in tests/sites") {

    auto client = GeminiClient {};

    for(const auto& current: std::filesystem::directory_iterator("tests/sites")) {
        auto ln = Link{"=> " + current.path().string()};
        Site* s = client.fetchSite(ln);
        std::string expectedSite = readFileToString(current.path().string());
        REQUIRE(s->getBody() == expectedSite);
        delete s;
    }
}

TEST_CASE("Match status code on .gmi files in tests/sites") {

    auto client = GeminiClient {};

    for(const auto& current: std::filesystem::directory_iterator("tests/sites")) {
        auto ln = Link{"=> " + current.path().string()};
        Site* s = client.fetchSite(ln);
        REQUIRE(s->getStatusCode() == 20);
        delete s;
    }
}

TEST_CASE("Match status code on missing .gmi files") {
    auto client = GeminiClient {};
    auto ln = Link{"=> tests/sites/this_file_doesnt_exist.gmi"};
    Site* s = client.fetchSite(ln);
    REQUIRE(s->getStatusCode() == 51);
    delete s;
}


TEST_CASE("Send basic gemini requests") {
    auto client = GeminiClient {};
    auto ln = Link{"=> gemini://laack.co"};
    Site* s = client.fetchSite(ln);
    REQUIRE(s->getStatusCode() == 20);
    REQUIRE(s->getHeader() == "20 text/gemini;lang=en-US\r\n");
    delete s;
}


TEST_CASE("Send request to non-existent gemini server") {
    auto client = GeminiClient {};
    auto ln = Link{"=> gemini://this.cant.be.a.site123"};
    Site* s = client.fetchSite(ln);
    REQUIRE(s == nullptr);
}
