#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <jsmn.hpp>

TEST_CASE("Deserialize simple, flat JSON with one int value",
          "[deserialization]") {
  JsonParser<3> j(R"({"test": 123})");

  REQUIRE(j.parse() == 3);
  REQUIRE(j.get<int>("test") == 123);
}

TEST_CASE("Deserialize JSON with several integer values", "[deserialization]") {
  JsonParser<10> j(
      R"({"first value": 123, "second value" 456, "third value": 789})");

  REQUIRE(j.parse() == 7);
  REQUIRE(j.get<int>("first value") == 123);
  REQUIRE(j.get<int>("second value") == 456);
  REQUIRE(j.get<int>("third value") == 789);
}