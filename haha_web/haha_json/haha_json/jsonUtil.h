#ifndef __HAHA_JSON_UTIL_H__
#define __HAHA_JSON_UTIL_H__

#include <string>
#include <string.h>
#include <sstream>
#include <bitset>
#include <locale>

namespace haha
{

namespace json
{

namespace util
{

inline bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
inline bool isNumber(char c) { return c >= '0' && c <= '9'; }
inline bool isNumberComponent(char c){
    return isdigit(c) || c == 'e' || c == 'E' || c == '-' || c == '.';
}
inline bool isHex(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}
inline bool isOct(char c) { return c >= '0' && c <= '7'; }
inline bool isBin(char c) { return c == '0' || c == '1'; }
inline bool isExponent(char c) { return c == 'e' || c == 'E'; }

inline bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\v'; }
inline bool isCtrlAndSpace(char c) { return c <= 32; }
inline bool isNewline(char c) { return c == '\n'; }

inline bool is_objectbegin(char c) { return c == '{'; }
inline bool is_objectend(char c) { return c == '}'; }
inline bool is_arraybegin(char c) { return c == '['; }
inline bool is_arrayend(char c) { return c == ']'; }
inline bool is_string_begin_end(char c) { return c == '"'; }

inline bool is_valid_next_escape_character(char c) {
    switch (c) 
    {
    case 't':
    case 'n':
    case '0':
    case 'b':
    case 'f':
    case 'x':
    case 'u':
    case 'r':
    case '\"':
    case '\\':
        return true;
    default:
        break;
    }
    return false;
}

enum class JSON_TAG{
    JSON_OBJECT_BEGIN = '{',
    JSON_OBJECT_END = '}',
    JSON_ARRAY_BEGIN = '[',
    JSON_ARRAY_END = ']',
    JSON_KEY_VALUE_SEP = ':',
    JSON_SEP = ',',
    JSON_STRING_TOKEN = '"',
};

inline std::string repeat_char(size_t cnt, char c){ 
    return std::string(cnt, c);
}

const char* skip_CtrlAndSpace(const char* str, size_t length, size_t offset=0);
std::string_view skip_CtrlAndSpace(std::string_view str, size_t offset=0);

const char* skip_utf8_bom(const char* str, size_t length, size_t offset=0);
std::string_view skip_utf8_bom(std::string_view str, size_t offset=0);

template <class T, size_t Base> inline T toBase(const std::string &s) {
    if (Base == 2){
        return std::bitset<32>(s).to_ulong();
    }

    std::stringstream ss;
    T val;
    if constexpr (Base == 16){
        ss << std::hex;
    }
    else if constexpr (Base == 8){
        ss << std::oct;
    }
    ss << s;
    ss >> val;
    return val;
}

unsigned int parse_hex4(const std::string &s);

unsigned int parse_hex4(std::string_view input);

unsigned int parse_hex4(const unsigned char * const input);

std::string utf16_literal_to_utf8(std::string_view &str);

}

}

}

#endif