#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <cstring>
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
  REQUIRE(j.get<long>("second value") == 456);
  REQUIRE(j.get<unsigned long long>("third value") == 789);
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

TEST_CASE("Check if JsonParser returns default value when no key is found",
          "[deserialization]") {
  JsonParser<1> j("{}");

  REQUIRE(j.parse() == 1);
  REQUIRE(j.get<bool>("test") == false);
  REQUIRE(j.get<int>("test") == 0);
  REQUIRE(j.get<unsigned>("test") == 0u);
  REQUIRE(j.get<float>("test") == 0.f);
  REQUIRE(j.get<double>("test") == 0.);
}

TEST_CASE("No-copy string parsing test", "[deserialization]") {
  JsonParser<5> j(
      R"({"TestString1": "This is a test", "teststr": "another test string"})");

  REQUIRE(j.parse() == 5);

  auto testString1View = j.get<JsonStringView>("TestString1");
  auto teststrView = j.get<JsonStringView>("teststr");
  auto invalidView = j.get<JsonStringView>("does not exists");

  REQUIRE(testString1View.length == strlen("This is a test"));
  REQUIRE(strncmp(testString1View.str, "This is a test",
                  testString1View.length) == 0);

  REQUIRE(teststrView.length == strlen("another test string"));
  REQUIRE(strncmp(teststrView.str, "another test string", teststrView.length) ==
          0);

  REQUIRE(invalidView.length == 0);
  REQUIRE(invalidView.str == nullptr);
}

TEST_CASE("String parsing test, with copying", "[deserialization]") {
  JsonParser<5> j(
      R"({"TestString1": "This is a test", "teststr": "another test string"})");

  REQUIRE(j.parse() == 5);

  char testString1Buf[32];
  char teststrBuf[32];
  char notExistingBuf[32];

  REQUIRE(j.getString("TestString1", testString1Buf) == true);
  REQUIRE(j.getString("teststr", teststrBuf) == true);
  REQUIRE(j.getString("does not exists", notExistingBuf) == false);
  REQUIRE(j.getString("teststr", nullptr) == false);

  REQUIRE(strcmp(testString1Buf, "This is a test") == 0);
  REQUIRE(strcmp(teststrBuf, "another test string") == 0);
}