#ifndef JSMN_WRAPPER_HPP
#define JSMN_WRAPPER_HPP
#include <cstring>

#include "jsmn.h"

template <unsigned MaxTokenCount>
class JsonParser {
 public:
  JsonParser() = default;
  JsonParser(char const* json) { setJsonString(json); }

  template <unsigned MaxTokenCountOfCopy>
  JsonParser(JsonParser<MaxTokenCountOfCopy> const& other) {
    setJsonString(other.mJson);
  }

  template <unsigned MaxTokenCountOfCopy>
  JsonParser& operator=(JsonParser<MaxTokenCountOfCopy> const& other) {
    setJsonString(other.mJson);
    std::memcpy(other.mTokens, mTokens,
                sizeof(jsmntok_t) * MaxTokenCountOfCopy);
    std::memcpy(&(other.mParser), &mParser, sizeof(jsmn_parser));
  }

  unsigned maxTokenCount() const { return MaxTokenCount; }

  void setJsonString(char const* json) { mJson = json; }
  char const* jsonString() const { return mJson; }

 private:
  jsmntok_t mTokens[MaxTokenCount]{};
  jsmn_parser mParser{};

  char const* mJson{nullptr};
  bool mParsed;
};

#endif