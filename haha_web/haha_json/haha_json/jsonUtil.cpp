#include <assert.h>
#include "jsonUtil.h"

using namespace haha::json;

const char* util::skip_CtrlAndSpace(const char* str, size_t length, size_t offset){
    if(str == nullptr || length <= offset){
        return nullptr;
    }
    str += offset;
    length -= offset;
    // ascll码32之前的为控制字符，32为空格符
    while(length && isCtrlAndSpace(*str)){
        --length;
        ++str;
    }
    return str;
}

std::string_view util::skip_CtrlAndSpace(std::string_view str, size_t offset){
    if(str.size() <= offset){
        return "";
    }
    str.remove_prefix(offset);
    while(!str.empty() && isCtrlAndSpace(str[0])){
        str.remove_prefix(1);
    }
    return str;
}

const char * util::skip_utf8_bom(const char* str, size_t length, size_t offset){
    if(str == nullptr || length <= offset){
        return nullptr;
    }
    str += offset;
    length -= offset;
    // ascll码32之前的为控制字符，32为空格符
    if(length >= 3 && (strncmp(str, "\xEF\xBB\xBF", 3) == 0)){
        str += 3;
    }
    return str;
}

std::string_view util::skip_utf8_bom(std::string_view str, size_t offset){
    if(str.size() <= offset){
        return "";
    }
    str.remove_prefix(offset);
    if(!str.empty() && (str.compare(0, 3, "\xEF\xBB\xBF") == 0)){
        str.remove_prefix(3);
    }
    return str;
}


unsigned int util::parse_hex4(const std::string &input){
    return util::parse_hex4(std::string_view(input));
}


unsigned int util::parse_hex4(std::string_view input){
    unsigned int h = 0;
    size_t i = 0;

    for (i = 0; i < 4; i++)
    {
        unsigned char uc = static_cast<unsigned char>(input[i]);
        /* parse digit */
        if ((uc >= '0') && (uc <= '9'))
        {
            h += (unsigned int) uc - '0';
        }
        else if ((uc >= 'A') && (uc <= 'F'))
        {
            h += (unsigned int) 10 + uc - 'A';
        }
        else if ((uc >= 'a') && (uc <= 'f'))
        {
            h += (unsigned int) 10 + uc - 'a';
        }
        else /* invalid */
        {
            return 0;
        }

        if (i < 3)
        {
            /* shift left to make place for the next nibble */
            h = h << 4;
        }
    }

    return h;
}


unsigned int util::parse_hex4(const unsigned char * const input)
{
    unsigned int h = 0;
    size_t i = 0;

    for (i = 0; i < 4; i++)
    {
        /* parse digit */
        if ((input[i] >= '0') && (input[i] <= '9'))
        {
            h += (unsigned int) input[i] - '0';
        }
        else if ((input[i] >= 'A') && (input[i] <= 'F'))
        {
            h += (unsigned int) 10 + input[i] - 'A';
        }
        else if ((input[i] >= 'a') && (input[i] <= 'f'))
        {
            h += (unsigned int) 10 + input[i] - 'a';
        }
        else /* invalid */
        {
            return 0;
        }

        if (i < 3)
        {
            /* shift left to make place for the next nibble */
            h = h << 4;
        }
    }

    return h;
}

std::string util::utf16_literal_to_utf8(std::string_view &str)
{
    // utf
    long unsigned int codepoint = 0;
    size_t utf8_length = 0;
    unsigned char first_byte_mark = 0;

    assert(str.size() > 2);
    assert(str[0] == '\\');
    assert(str[1] == 'u');

    str.remove_prefix(2);
    if(str.size() < 4){
        return "";
    }
    unsigned int first_code = 0;
    std::string_view first_seq = str.substr(0, 4);
    first_code = util::parse_hex4(first_seq);
    if (((first_code >= 0xDC00) && (first_code <= 0xDFFF)))
    {
        return "";
    }
    str.remove_prefix(4);

    // utf16
    /* \uXXXX\uXXXX */
    unsigned int second_code = 0;
    if ((first_code >= 0xD800) && (first_code <= 0xDBFF)){
        if(str.size() < 6 || str[0] != '\\' || str[1] != 'u'){
            return "";
        }
        str.remove_prefix(2);
        std::string_view second_seq = str.substr(0, 4);
        second_code = util::parse_hex4(second_seq);
        if ((second_code < 0xDC00) || (second_code > 0xDFFF))
        {
            /* invalid second half of the surrogate pair */
            return "";
        }
        codepoint = 0x10000 + (((first_code & 0x3FF) << 10) | (second_code & 0x3FF));
        str.remove_prefix(4);
    }
    // utf8
    /* \uXXXX */
    else{
        codepoint = first_code;
    }

    /* encode as UTF-8
    * takes at maximum 4 bytes to encode:
    * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
    if (codepoint < 0x80)
    {
        /* normal ascii, encoding 0xxxxxxx */
        utf8_length = 1;
    }
    else if (codepoint < 0x800)
    {
        /* two bytes, encoding 110xxxxx 10xxxxxx */
        utf8_length = 2;
        first_byte_mark = 0xC0; /* 11000000 */
    }
    else if (codepoint < 0x10000)
    {
        /* three bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx */
        utf8_length = 3;
        first_byte_mark = 0xE0; /* 11100000 */
    }
    else if (codepoint <= 0x10FFFF)
    {
        /* four bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx 10xxxxxx */
        utf8_length = 4;
        first_byte_mark = 0xF0; /* 11110000 */
    }
    else
    {
        /* invalid unicode codepoint */
        return "";
    }

    std::string output(utf8_length, '\0');
    for (auto utf8_position = utf8_length - 1; utf8_position > 0; utf8_position--)
    {
        /* 10xxxxxx */
        output[utf8_position] += (unsigned char)((codepoint | 0x80) & 0xBF);
        codepoint >>= 6;
    }

    if (utf8_length > 1)
    {
        output[0] = (unsigned char)((codepoint | first_byte_mark) & 0xFF);
    }
    else
    {
        output[0] = (unsigned char)(codepoint & 0x7F);
    }

    return output;
}