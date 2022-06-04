#ifndef __HAHA_HTTPREQUEST_H__
#define __HAHA_HTTPREQUEST_H__

#include <string_view>
#include "base/Buffer.h"
#include "http/HttpUtil.h"
#include "http/HttpUrl.h"
#include "http/HttpCookie.h"
#include "http/HttpMultiPart.h"
#include "base/Log.h"
#include "http/HttpHeader.h"
// #include "base/StringView.h"

namespace haha{

class HttpRequest{
public:
    enum CHECK_STATE
    {
        CHECK_REQUESTLINE,
        CHECK_HEADER,
        CHECK_CONTENT,
        CHECK_DONE,
    };
    enum RET_STATE
    {
        OK_REQUEST,
        AGAIN_REQUEST,
        BAD_REQUEST,
    };
    enum REASON{
        BAD_REQUESTLINE,
        BAD_HEADER,
        EXCEED_REQUESTLINE,
        EXCEED_HEADER,
    };

public:
    HttpRequest(Buffer::ptr data);
    RET_STATE parseRequest();
    HttpHeader getHeader() { return header_; }
    Buffer::ptr getBuffer() { return data_; }
    std::string getBody() { return body_; }

    CHECK_STATE getState() { return state_; }
    RET_STATE getRetState() { return retState_; }
    bool complete() { return state_ == CHECK_DONE; }

    bool keepAlive() { return keepAlive_; }
    bool hasCookies() { return hasCookies_; }

// private:
    RET_STATE parseRequestLine();
    RET_STATE parseRequestHeader();
    RET_STATE parseRequestContent();

    void parseKeepAlive();
    void parseContentType();
    void parseAcceptEncoding();
    void parseCookies();
    void parseContentLength();
    void parseTransferEncoding();

    RET_STATE parseChunked();

private:
    Buffer::ptr data_;
    CHECK_STATE state_;
    RET_STATE retState_;

    // 记录上一次数据解析结束的位置
    size_t lastEnd_;

    const size_t max_requestLine_len_;
    const size_t max_requestHeader_len_;
    const size_t chunk_size_str_limit;

    HttpMethod method_;
    HttpUrl reqUrl_;
    HttpVersion version_;
    HttpHeader header_;
    HttpCookie cookies_;
    HttpMultiPart multipart_;
    std::string body_;
    HttpForm form_;

    HttpContentType contentType_;
    size_t contentLength_;

    std::string transferEncoding_;
    bool chunked_;

    HttpChunkedState cur_chunkedState_;
    int cur_chunk_size_;

    bool keepAlive_;
    bool compressed_;
    bool hasContentType_;
    bool hasCookies_;
    bool hasContentLength_;
    bool hasTransferEncoding_;
};

}


#endif