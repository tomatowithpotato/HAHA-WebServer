#ifndef __HAHA_HTTPTEST_H__
#define __HAHA_HTTPTEST_H__

#include <string>
#include <vector>
#include "http/HttpUtil.h"
#include "base/util.h"
#include "base/EncodeUtil.h"

namespace haha{

// 测试专用
namespace test{

std::string genRandomStr(size_t n);

std::string genRandomKVStr(size_t n, const std::string &join, const std::string &split, bool urlencode=false);

std::string genRandomKVStrs(size_t n, const std::string &join, const std::string &split, bool urlencode=false);

std::vector<std::string> randomSplitStr(const std::string &str, size_t limit = -1);

class RandomHttpRequestString{
public:
    RandomHttpRequestString();

    std::string create_random_RequestLine();

    std::string create_random_RequestHeader(const std::string &requestBody, const std::string &contentType, bool chunked);

    std::tuple<std::string, std::string, bool> create_random_RequestBody();

    std::string getRequestLine() const { return requestLine_; }
    std::string getRequestHeader() const { return requestHeader_; }
    std::string getRequestBody() const { return requestBody_; }
    std::string getRequest() const { return request_;}

    std::string getContentType() const { return contentType_; }

private:
    std::string requestLine_;
    std::string requestHeader_;
    std::string requestBody_;
    std::string request_;
    std::string contentType_;
    bool chunked_;
};

}

}

#endif