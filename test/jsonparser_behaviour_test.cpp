#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <jsmn.hpp>

TEST_CASE("Default construction test", "[construciton]") {
  JsonParser<1> j;

  REQUIRE(j.parsed() == false);
  REQUIRE(j.maxTokenCount() == 1);
  REQUIRE(j.jsonString() == nullptr);
}

TEST_CASE("Construction with c-string constant, no parsing", "[construciton]") {
  char const* data = R"({"test": 123})";
  JsonParser<3> j(data);

  REQUIRE(j.parsed() == false);
  REQUIRE(j.maxTokenCount() == 3);
  REQUIRE(j.jsonString() == data);
}

TEST_CASE("Construction with a c-string constant, then parsing",
          "[construction]") {
  char const* data = R"({"test": 123})";
  JsonParser<3> j(data);
  j.parse();

  REQUIRE(j.parse() == 3);
  REQUIRE(j.maxTokenCount() == 3);
  REQUIRE(j.jsonString() == data);
}

TEST_CASE("Default construction, then string set and parse", "[construction]") {
  char const* data = R"({"test": 123})";
  JsonParser<3> j;
  j.setJsonString(data);

  REQUIRE(j.parse() == 3);
  REQUIRE(j.maxTokenCount() == 3);
  REQUIRE(j.jsonString() == data);
}

TEST_CASE("Copy c-tor with no init", "[copying]") {
  JsonParser<1> j1;
  JsonParser<1> j2(j1);

  REQUIRE(j1.parsed() == false);
  REQUIRE(j2.parsed() == false);
  REQUIRE(j1.maxTokenCount() == 1);
  REQUIRE(j2.maxTokenCount() == 1);
  REQUIRE(j1.jsonString() == nullptr);
  REQUIRE(j2.jsonString() == nullptr);
}

TEST_CASE("Copy operator with no init", "[copying]") {
  JsonParser<1> j1;
  JsonParser<1> j2;

  j2 = j1;

  REQUIRE(j1.parsed() == false);
  REQUIRE(j2.parsed() == false);
  REQUIRE(j1.maxTokenCount() == 1);
  REQUIRE(j2.maxTokenCount() == 1);
  REQUIRE(j1.jsonString() == nullptr);
  REQUIRE(j2.jsonString() == nullptr);
}

TEST_CASE("Copy c-tor with init, no parse", "[copying]") {
  char const* data = R"({"test": 123})";

  JsonParser<3> j1(data);
  JsonParser<3> j2(j1);

  REQUIRE(j1.parsed() == false);
  REQUIRE(j2.parsed() == false);
  REQUIRE(j1.maxTokenCount() == 3);
  REQUIRE(j2.maxTokenCount() == 3);
  REQUIRE(j1.jsonString() == data);
  REQUIRE(j2.jsonString() == data);
}

TEST_CASE("Copy operator with init, no parse", "[copying]") {
  char const* data_1 = R"({"test1": 123})";
  char const* data_2 = R"({"test2": 456})";

  JsonParser<3> j1(data_1);
  JsonParser<3> j2(data_2);

  j2 = j1;

  REQUIRE(j1.parsed() == false);
  REQUIRE(j2.parsed() == false);
  REQUIRE(j1.maxTokenCount() == 3);
  REQUIRE(j2.maxTokenCount() == 3);
  REQUIRE(j1.jsonString() == data_1);
  REQUIRE(j2.jsonString() == data_1);
}

TEST_CASE("Copy c-tor with parsing", "[copying]") {
  char const* data = R"({"test": 123})";

  JsonParser<3> j1(data);
  REQUIRE(j1.parse() == 3);
  REQUIRE(j1.parsed() == true);
  REQUIRE(j1.maxTokenCount() == 3);
  REQUIRE(j1.jsonString() == data);

  JsonParser<3> j2(j1);

  REQUIRE(j2.parsed() == true);
  REQUIRE(j2.maxTokenCount() == 3);
  REQUIRE(j2.jsonString() == data);

  REQUIRE(j1.get<int>("test") == 123);
  REQUIRE(j2.get<int>("test") == 123);
}

TEST_CASE("Copy operator with parsing", "[copying]") {
  char const* data = R"({"test": 123})";

  JsonParser<3> j1(data);
  REQUIRE(j1.parse() == 3);
  REQUIRE(j1.parsed() == true);
  REQUIRE(j1.maxTokenCount() == 3);
  REQUIRE(j1.jsonString() == data);

  JsonParser<3> j2;

  j2 = j1;
  REQUIRE(j2.parsed() == true);
  REQUIRE(j2.maxTokenCount() == 3);
  REQUIRE(j2.jsonString() == data);

  REQUIRE(j1.get<int>("test") == 123);
  REQUIRE(j2.get<int>("test") == 123);
}