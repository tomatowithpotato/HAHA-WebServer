#include "base/EncodeUtil.h"

namespace haha{

/* ************************************************解码************************************************ */

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


/* ************************************************编码************************************************ */

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