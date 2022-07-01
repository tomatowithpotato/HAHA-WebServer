#include "base/util.h"
#include <algorithm>
#include <assert.h>

namespace haha{

pid_t GetThreadId(){
    return syscall(SYS_gettid);
}

// 十六进制数转十进制数
unsigned char hex2dec(char c) {
    if (c >= '0' && c <= '9')
        return c - '0'; // [0-9]
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 10; // a-f [10-15]
    else if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10; // A-F [10-15]
    return c;
}

// 十进制数转十六进制数
char dec2hex(char c) {
    if (c >= 0 && c <= 9)
        return c + '0'; // ['0'-'9']
    else if (c >= 10 && c <= 15)
        return c - 10 + 'A'; // ['A'-'Z']
    return c;
}

// 十六进制字符串转为int，如果转换失败返回-1
int hexString2Int(const std::string& hexStr){
    int res = 0;
    for(const auto &c : hexStr){
        if(!isxdigit(c)){
            return - 1;
        }
        res = res*10 + hex2dec(c);
        if(res < 0){
            return -1;
        }
    }
    return res;
}

// 十六进制字符串转为int，如果转换失败返回-1
int hexString2Int(std::string_view hexStr){
    int res = 0;
    for(const auto &c : hexStr){
        if(!isxdigit(c)){
            return - 1;
        }
        res = res*10 + hex2dec(c);
        if(res < 0){
            return -1;
        }
    }
    return res;
}


std::string int2HexString(int dec){
    std::string hex;
    while(dec){
        hex += dec2hex(dec % 10);
        dec /= 10;
    }
    std::reverse(hex.begin(), hex.end());
    return hex;
}


void toLowers(char *s, size_t size) {
  for (size_t i = 0; i < size; i++)
    if (isupper(s[i]))
      s[i] = tolower(s[i]);
}

void toUppers(char *s, size_t size) {
  for (size_t i = 0; i < size; i++)
    if (islower(s[i]))
      s[i] = toupper(s[i]);
}

std::string toLowers(const std::string &s) {
    std::string ls(s);
    toLowers(ls.data(), ls.size());
    return ls;
}

std::string toUppers(const std::string &s) {
    std::string us(s);
    toUppers(us.data(), us.size());
    return us;
}

bool stringCaseCmp(const std::string &a, const std::string &b){
    return toLowers(a) == toLowers(b);
}

}