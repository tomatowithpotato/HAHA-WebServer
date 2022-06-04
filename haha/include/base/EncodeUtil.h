#ifndef __HAHA_ENCODEUTIL_H__
#define __HAHA_ENCODEUTIL_H__

#include <string>
#include <string_view>
#include <assert.h>
#include <string.h>

#include "base/util.h"

namespace haha{

namespace EncodeUtil{


/* 
字符'a'-'z','A'-'Z','0'-'9','.','-','*'和'_' 都不被编码，维持原值；
空格' '被转换为加号'+'。
其他每个字节都被表示成"%XY"的格式，X和Y分别代表一个十六进制位。编码为UTF-8。 
*/

std::string urlDecode(const char *, size_t);
std::string urlDecode(std::string_view);
std::string urlDecode(const char *);

std::string urlEncode(const char *, size_t);
std::string urlEncode(std::string_view);
std::string urlEncode(const char *);

}

}

#endif