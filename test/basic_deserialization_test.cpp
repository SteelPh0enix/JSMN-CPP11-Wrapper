#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <jsmn.hpp>

TEST_CASE("Deserialize simple, flat JSON with one value", "[deserialization]") {
  JsonParser<1> j(R"({"test": 123})");

  REQUIRE(j.parse() == 0);
  REQUIRE(j.get<int>("test") == 123);
}