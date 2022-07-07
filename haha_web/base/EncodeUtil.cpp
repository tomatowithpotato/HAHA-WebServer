#include "base/EncodeUtil.h"

namespace haha{


unsigned int EncodeUtil::murmurHash2(const std::string& s){
    constexpr unsigned int m = 0x5bd1e995;
    constexpr int r = 24;
    // Initialize the hash to a 'random' value
    size_t len = s.size();
    unsigned int h = 0xEE6B27EB ^ len;

    // Mix 4 bytes at a time into the hash

    const unsigned char *data = (const unsigned char *)s.data();

    while (len >= 4) {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array
    switch (len) {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}


/* ************************************************url解码************************************************ */

std::string EncodeUtil::urlDecode(const char *value, size_t size) {
    std::string escaped;
    for (size_t i = 0; i < size; ++i) {
        if (value[i] == '%' && i + 2 < size && isxdigit(value[i + 1]) &&
            isxdigit(value[i + 2])) {
            // merge two char to one byte
            // %AB => 0xAB
            unsigned char byte =
                ((unsigned char)hex2dec(value[i + 1]) << 4) | hex2dec(value[i + 2]);
            escaped += byte;
            i += 2;
        }
        else if(value[i] == '%'){
            // 解码失败，返回空
            return "";
        }
        else if(value[i] == '+'){
            escaped += ' ';
        } 
        else {
            escaped += value[i];
        }
    }
    return escaped;
}

std::string EncodeUtil::urlDecode(std::string_view value) {
    return urlDecode(value.data(), value.size());
}

std::string EncodeUtil::urlDecode(const char *value) {
    return urlDecode(value, strlen(value));
}


/* ************************************************url编码************************************************ */

std::string EncodeUtil::urlEncode(const char *value, size_t size) {
    std::string escaped;

    for (size_t i = 0; i < size; i++) {
        unsigned char c = (unsigned char)value[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped += c;
        }
        // 可以把' '转为'+'，也可以选择直接转为十六进制编码
        // else if(c == ' '){
        //     escaped += '+';
        // }
        else{
            // split one byte to two char
            // 0xAB => %AB
            char buf[5]{0};
            sprintf(buf, "%%%c%c", toupper(dec2hex(c >> 4)), toupper(dec2hex(c & 15)));
            escaped += buf;
        }
    }

    return escaped;
}

std::string EncodeUtil::urlEncode(std::string_view value) {
  return urlEncode(value.data(), value.size());
}

std::string EncodeUtil::urlEncode(const char *value) {
  return urlEncode(value, strlen(value));
}

}