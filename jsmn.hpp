#ifndef JSMN_WRAPPER_HPP
#define JSMN_WRAPPER_HPP
#include <string.h>

#include "jsmn.h"

struct JsonStringView {
  char const* str;
  size_t length;
};
namespace {
template <typename T>
void ignore(T&&) {}
}  // namespace

// Default behaviour is to return new, empty object, if there is no available
// overload
// This function should parse JSON based on token and JSON string, and return
// value of demanded type. Feel free to overload for your own types.
template <typename T>
T parseJsonValue(jsmntok_t const& token, char const* json) {
  // suppress "unused argument" warnings
  ignore(token);
  ignore(json);
  return T{};
}

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
    for (auto* token = mTokens; token < mTokens + MaxTokenCount; token++) {
      const size_t tokenLength = token->end - token->start;
      if (token->type == JSMN_STRING && strlen(key) == tokenLength &&
          strncmp(mJson + token->start, key, tokenLength) == 0) {
        return parseJsonValue<T>(*(token + 1), mJson);
      }
    }

    // Return default value of type if no key was found
    return T{};
  }

 private:
  jsmntok_t mTokens[MaxTokenCount]{};
  jsmn_parser mParser{};

  char const* mJson{nullptr};
  bool mParsed;
};

template <>
long parseJsonValue(jsmntok_t const& token, char const* json) {
  return strtol(json + token.start, nullptr, 10);
}

template <>
int parseJsonValue(jsmntok_t const& token, char const* json) {
  // I'll just use `strtol` for every int-like overload, since it's pretty
  // stable, leaving type size management to user.
  return static_cast<int>(parseJsonValue<long>(token, json));
}

template <>
short parseJsonValue(jsmntok_t const& token, char const* json) {
  return static_cast<short>(parseJsonValue<long>(token, json));
}

template <>
long long parseJsonValue(jsmntok_t const& token, char const* json) {
  // In this case i have to use strtoll
  return strtoll(json + token.start, nullptr, 10);
}

template <>
unsigned long parseJsonValue(jsmntok_t const& token, char const* json) {
  // Different function for unsigneds, same stuff for rest of unsigned-like
  // types
  return strtoul(json + token.start, nullptr, 10);
}

template <>
unsigned int parseJsonValue(jsmntok_t const& token, char const* json) {
  // I'll just use `strtol` for every int-like overload, since it's pretty
  // stable, leaving type size management to user.
  return static_cast<unsigned int>(parseJsonValue<unsigned long>(token, json));
}

template <>
unsigned short parseJsonValue(jsmntok_t const& token, char const* json) {
  return static_cast<unsigned short>(
      parseJsonValue<unsigned long>(token, json));
}

template <>
unsigned long long parseJsonValue(jsmntok_t const& token, char const* json) {
  // In this case i have to use strtoull
  return strtoull(json + token.start, nullptr, 10);
}

template <>
bool parseJsonValue(jsmntok_t const& token, char const* json) {
  return strncmp(json + token.start, "true", token.end - token.start) == 0;
}

template <>
float parseJsonValue(jsmntok_t const& token, char const* json) {
  return strtof(json + token.start, nullptr);
}

template <>
double parseJsonValue(jsmntok_t const& token, char const* json) {
  return strtod(json + token.start, nullptr);
}

template <>
long double parseJsonValue(jsmntok_t const& token, char const* json) {
  return strtold(json + token.start, nullptr);
}

#endif