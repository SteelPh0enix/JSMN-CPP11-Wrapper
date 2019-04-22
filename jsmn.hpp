#ifndef JSMN_WRAPPER_HPP
#define JSMN_WRAPPER_HPP
#include <stdlib.h>
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
  int length;
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
///
/// Constrains: Every key and string value must be unique. Maybe i will change
/// this in future, but - since i expect this class to be used with simple
/// JSONs, in restricted environment - i prefer to keep it as simple as
/// possible.
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
  /// it will return empty, default- or zero-initialized object.
  /// Complexity: linear
  template <typename T>
  T get(char const* key) {
    TokenPair tokens = getTokenPair(key);
    if (tokens.name == nullptr || tokens.value == nullptr) {
      // Return default value of type if no key was found
      return T{};
    }

    return parseJsonValue<T>(*(tokens.value), jsonString());
  }

  /// Copies requested string from JSON to buffer passed as argument and returns
  /// `true`. If `key` wasn't found in JSON, value is empty, or pointer to
  /// buffer is invalid, does nothing and returns `false`. Complexity: linear
  bool getString(char const* key, char* buffer) {
    if (buffer == nullptr) {
      return false;
    }

    JsonStringView str = get<JsonStringView>(key);
    if (str.length == 0) {
      return false;
    }

    memcpy(buffer, str.str, str.length);
    return true;
  }

  /// Copies an array of values from JSON to buffer. If `key` wasn't found in
  /// JSON, array is empty, or pointer to buffer is invalid, does nothing and
  /// returns `false`. Complexity: linear
  /// Should work with all types which have specialization for parseJsonValue.
  template <typename T>
  bool getArray(char const* key, T* buffer) {
    if (buffer == nullptr) {
      return false;
    }

    TokenPair arrayTokens = getTokenPair(key);
    if (arrayTokens.value == nullptr) {
      return false;
    }

    for (int i = 0; i < arrayTokens.value->size; i++) {
      buffer[i] = parseJsonValue<T>(*(arrayTokens.value + i + 1), jsonString());
    }

    return true;
  }

 private:
  struct TokenPair {
    jsmntok_t const* name;
    jsmntok_t const* value;
  };

  TokenPair getTokenPair(char const* key) {
    if (parsed()) {
      // Linear search for the key
      for (auto* token = mTokens; token < mTokens + MaxTokenCount; token++) {
        const size_t tokenLength = token->end - token->start;
        if (token->type == JSMN_STRING && strlen(key) == tokenLength &&
            strncmp(mJson + token->start, key, tokenLength) == 0) {
          return TokenPair{token, (token + 1)};
        }
      }
    }

    return TokenPair{nullptr, nullptr};
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
  if (token.type == JSMN_PRIMITIVE) {
    return strtol(json + token.start, nullptr, 10);
  }
  return 0l;
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
  if (token.type == JSMN_PRIMITIVE) {
    return strtoll(json + token.start, nullptr, 10);
  }
  return 0ll;
}

template <>
unsigned long parseJsonValue(jsmntok_t const& token, char const* json) {
  // Different function for unsigneds, same stuff for rest of unsigned-like
  // types
  if (token.type == JSMN_PRIMITIVE) {
    return strtoul(json + token.start, nullptr, 10);
  }
  return 0ul;
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
  if (token.type == JSMN_PRIMITIVE) {
    return strtoull(json + token.start, nullptr, 10);
  }
  return 0ull;
}

template <>
bool parseJsonValue(jsmntok_t const& token, char const* json) {
  if (token.type == JSMN_PRIMITIVE) {
    return strncmp(json + token.start, "true", token.end - token.start) == 0;
  }
  return false;
}

template <>
float parseJsonValue(jsmntok_t const& token, char const* json) {
  if (token.type == JSMN_PRIMITIVE) {
    return strtof(json + token.start, nullptr);
  }
  return 0.f;
}

template <>
double parseJsonValue(jsmntok_t const& token, char const* json) {
  if (token.type == JSMN_PRIMITIVE) {
    return strtod(json + token.start, nullptr);
  }
  return 0.;
}

template <>
long double parseJsonValue(jsmntok_t const& token, char const* json) {
  if (token.type == JSMN_PRIMITIVE) {
    return strtold(json + token.start, nullptr);
  }
  return 0.;
}

template <>
JsonStringView parseJsonValue(jsmntok_t const& token, char const* json) {
  if (token.type == JSMN_STRING) {
    return {json + token.start, token.end - token.start};
  }
  return JsonStringView{};
}

template <>
char parseJsonValue(jsmntok_t const& token, char const* json) {
  // This is just another integer conversion
  return static_cast<char>(parseJsonValue<long>(token, json));
}

template <>
unsigned char parseJsonValue(jsmntok_t const& token, char const* json) {
  // This is just another integer conversion
  return static_cast<char>(parseJsonValue<unsigned long>(token, json));
}

#endif