#ifndef JSMN_WRAPPER_HPP
#define JSMN_WRAPPER_HPP
#include <string.h>

#include "jsmn.h"

/// Structure containing pointer to first character of searched string and it's
/// length
/// IMPORTANT: Do not use `str` as normal c-style string, since it's not
/// null-terminated for the value. It's just a pointer to the beginning of
/// searched string, making it suitable for no-copy operations. If you want to
/// get a copy of string, use `JsonParser::getString` method.
struct JsonStringView {
  char const* str;
  size_t length;
};

namespace {
template <typename T>
void ignore(T&&) {}
}  // namespace

/// Template function used by `JsonParser::get` to parse values by types.
/// Specialize it, if you need custom type support. By default, it returns
/// empty, zero-initialized (or default-initialized) object.
template <typename T>
T parseJsonValue(jsmntok_t const& token, char const* json) {
  // suppress "unused argument" warnings
  ignore(token);
  ignore(json);
  return T{};
}

/// Class, wrapping JSMN functionality into more ergonomic API.
/// Note: MaxTokenCount should be maximum token amount expected in JSON parsed
/// by instance of this class. Important: Every piece of JSON (keys, values,
/// arrays, objects) are tokens. So, for example, this json: {"test": 3, "a":
/// {"b": 5}} contains 7 tokens ("test", 3, "a", "b", 5, and two objects - first
/// with {"b": 5}, second with whole JSON).
template <size_t MaxTokenCount>
class JsonParser {
 public:
  /// Default constructor
  JsonParser() { jsmn_init(&mParser); }

  /// Constructor, initalizing parser with string. Does not automatically parse
  /// it, you still have to call `JsonParser::parse` manually.
  JsonParser(char const* json) {
    setJsonString(json);
    jsmn_init(&mParser);
  }

  /// Copy constructor
  JsonParser(JsonParser<MaxTokenCount> const& other) {
    jsmn_init(&mParser);
    setJsonString(other.mJson);
    memcpy(mTokens, other.mTokens, sizeof(jsmntok_t) * MaxTokenCount);
    memcpy(&mParser, &(other.mParser), sizeof(jsmn_parser));
    mParsed = other.mParsed;
  }

  /// Copy operator
  JsonParser& operator=(JsonParser<MaxTokenCount> const& other) {
    // JSMN parser should be initialized by now.
    setJsonString(other.mJson);
    memcpy(mTokens, other.mTokens, sizeof(jsmntok_t) * MaxTokenCount);
    memcpy(&mParser, &(other.mParser), sizeof(jsmn_parser));
    mParsed = other.mParsed;

    return *this;
  }

  /// Returns max token count, passed as template parameter
  unsigned maxTokenCount() const { return MaxTokenCount; }

  /// JSON string setter
  void setJsonString(char const* json) { mJson = json; }

  /// JSON string getter
  char const* jsonString() const { return mJson; }

  /// Inovkes JSMN to parse the JSON string.
  /// Returns amount of tokens parsed and sets `parsed` flag to true on success
  /// Returns negative value (`jsmnerr` enumeration from `jsmn.h`) on failure.
  int parse() {
    int retval =
        jsmn_parse(&mParser, mJson, strlen(mJson), mTokens, MaxTokenCount);
    mParsed = retval > 0;

    return retval;
  }

  /// Returns `true` if object contains parsed JSON
  bool parsed() const { return mParsed; }

  /// Searches for key in JSON and returns value of requested type. Uses
  /// `parseJsonValue` function to convert string to requested type. In case
  /// there is no specialization for used type, or requested key wasn't found,
  /// it will return empty, default- or zero-initialized object. Complexity:
  /// linear
  template <typename T>
  T get(char const* key) {
    TokenPair tokens = getTokenPair(key);
    if (tokens.name.type == JSMN_UNDEFINED) {
      // Return default value of type if no key was found
      return T{};
    }

    return parseJsonValue<T>(tokens.value, jsonString());
  }

  /// Copies requested string from JSON to buffer passed as argument and returns
  /// `true`. If `key` wasn't found in JSON, or value is empty, does nothing and
  /// returns `false`.
  bool getString(char const* key, char* buffer) {
    JsonStringView str = get<JsonStringView>(key);
    if (str.length == 0) {
      return false;
    }

    std::memcpy(buffer, str.str, str.length);
    return true;
  }

 private:
  struct TokenPair {
    jsmntok_t name;
    jsmntok_t value;
  };

  TokenPair getTokenPair(char const* key) {
    if (parsed()) {
      // Linear search for the key
      for (auto* token = mTokens; token < mTokens + MaxTokenCount; token++) {
        const size_t tokenLength = token->end - token->start;
        if (token->type == JSMN_STRING && strlen(key) == tokenLength &&
            strncmp(mJson + token->start, key, tokenLength) == 0) {
          return TokenPair{*token, *(token + 1)};
        }
      }
    }

    return TokenPair{};
  }

  jsmntok_t mTokens[MaxTokenCount]{};
  jsmn_parser mParser{};

  char const* mJson{nullptr};
  bool mParsed{false};
};

/*
  Specializations for parseJsonValue function.
*/

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

template <>
JsonStringView parseJsonValue(jsmntok_t const& token, char const* json) {
  return {json + token.start, token.size};
}

#endif