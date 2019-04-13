#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <jsmn.hpp>

TEST_CASE("Deserialize simple, flat JSON with one value", "[deserialization]") {
  Json<1> j(R"({"test": 123})");

  REQUIRE(j["test"] == 123);
}