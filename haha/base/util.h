#ifndef __HAHA_UTIL_H__
#define __HAHA_UTIL_H__

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <vector>
#include <string>

namespace haha{

// 十六进制数转十进制数
unsigned char hex2dec(char);

// 十进制数转十六进制数
char dec2hex(char);

// 十六进制字符串转为int，如果转换失败返回-1
int hexString2Int(const std::string& hexStr);
int hexString2Int(std::string_view hexStr);

// int转十六进制字符串
std::string int2HexString(int dec);

void toLowers(char *, size_t);
void toUppers(char *, size_t);
std::string toLowers(const std::string &);
std::string toUppers(const std::string &);

bool stringCaseCmp(const std::string &, const std::string &);

pid_t GetThreadId();

}

#endif