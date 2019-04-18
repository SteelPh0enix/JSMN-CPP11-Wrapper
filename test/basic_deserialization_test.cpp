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
  JsonParser<7> j(
      R"({"first value": 123, "second value" 456, "third value": 789})");

  REQUIRE(j.parse() == 7);
  REQUIRE(j.get<int>("first value") == 123);
  REQUIRE(j.get<int>("second value") == 456);
  REQUIRE(j.get<int>("third value") == 789);
}

TEST_CASE("Deserialize JSON with floating-point values", "[deserialization]") {
  JsonParser<7> j(
      R"({"test value": 12.34, "double": 3.4567898544, "int": 123})");

  REQUIRE(j.parse() == 7);
  REQUIRE(j.get<float>("test value") == 12.34f);
  REQUIRE(j.get<double>("double") == 3.4567898544);
}

TEST_CASE("Deserialize JSON with boolean values", "[deserialization]") {
  JsonParser<5> j(R"({"this is true": true, "this is false": false})");

  REQUIRE(j.parse() == 5);
  REQUIRE(j.get<bool>("this is true") == true);
  REQUIRE(j.get<bool>("this is false") == false);
}