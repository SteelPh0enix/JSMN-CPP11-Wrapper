#ifndef JSMN_WRAPPER_HPP
#define JSMN_WRAPPER_HPP
#include <string.h>

#include "jsmn.h"

template <size_t MaxTokenCount>
class JsonParser {
 public:
  JsonParser() { jsmn_init(&mParser); }
  JsonParser(char const* json) {
    setJsonString(json);
    jsmn_init(&mParser);
  }

  template <unsigned MaxTokenCountOfCopy>
  JsonParser(JsonParser<MaxTokenCountOfCopy> const& other) {
    setJsonString(other.mJson);
    jsmn_init(&mParser);
  }

  template <unsigned MaxTokenCountOfCopy>
  JsonParser& operator=(JsonParser<MaxTokenCountOfCopy> const& other) {
    // JSMN parser should be initialized by now
    setJsonString(other.mJson);
    std::memcpy(other.mTokens, mTokens,
                sizeof(jsmntok_t) * MaxTokenCountOfCopy);
    std::memcpy(&(other.mParser), &mParser, sizeof(jsmn_parser));
  }

  unsigned maxTokenCount() const { return MaxTokenCount; }

  void setJsonString(char const* json) { mJson = json; }
  char const* jsonString() const { return mJson; }

  int parse() {
    return jsmn_parse(&mParser, mJson, strlen(mJson), mTokens, MaxTokenCount);
  }

  // Note: all odd entities in mTokens should be keys, and even - values. At
  // least in case of flat JSONs.

  template <typename T>
  T get(char const* key) {
    // Linear search for the key
    for (auto* token = mTokens; token < mTokens + MaxTokenCount; token += 2) {
      const size_t tokenLength = token->end - token->start;
      if (token->type == JSMN_STRING && strlen(key) == tokenLength &&
          strncmp(mJson + token->start, key, tokenLength) == 0) {
        return parseJsonValue<T>(*(token + 1));
      }
    }
  }

 private:
  jsmntok_t mTokens[MaxTokenCount]{};
  jsmn_parser mParser{};

  char const* mJson{nullptr};
  bool mParsed;
};

template <typename T>
T parseJsonValue(jsmntok_t const& token) {}

#endif