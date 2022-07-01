#ifndef __HAHA_HTTPREQUEST_H__
#define __HAHA_HTTPREQUEST_H__

#include <string_view>
#include <memory>
#include "base/Buffer.h"
#include "net/TcpConnection.h"
#include "http/HttpUtil.h"
#include "http/HttpUrl.h"
#include "http/HttpCookie.h"
#include "http/HttpMultiPart.h"
#include "log/Log.h"
#include "http/HttpHeader.h"
#include "http/HttpSession.h"
// #include "base/StringView.h"

namespace haha{

class HttpRequest{
public:
    typedef std::shared_ptr<HttpRequest> ptr;
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
    HttpRequest(HttpSessionManager*, TcpConnection::ptr);
    RET_STATE parseRequest();

    HttpMethod getMethod() const { return method_; }
    const HttpUrl& getUrl() const { return reqUrl_; }
    HttpVersion getVersion() const { return version_; }
    const HttpHeader& getHeader() const { return header_; }
    Buffer::ptr getBuffer() const { return data_; }
    TcpConnection::ptr getConnection() const { return conn_; }
    const std::string& getBody() const { return body_; }

    bool getKeepAlive() const { return keepAlive_; }
    HttpContentType getContentType() const { return contentType_; }
    const std::string& getAcceptEncoding() const { return acceptEncoding_; }
    const HttpCookie& getCookie() const { return cookie_; }
    size_t getContentLength() const { return contentLength_; }
    const std::string& getTransferEncoding() const { return transferEncoding_; }

    CHECK_STATE getState() const { return state_; }
    RET_STATE getRetState() const { return retState_; }
    bool complete() const { return state_ == CHECK_DONE; }

    bool keepAlive() const { return keepAlive_; }
    bool hasCookies() const { return hasCookies_; }

    // 获取session
    HttpSession::ptr getSession(bool autoCreate = true);

private:
    RET_STATE parseRequestLine();
    RET_STATE parseRequestHeader();
    RET_STATE parseRequestContent();

    void parseKeepAlive();
    void parseContentType();
    void parseAcceptEncoding();
    void parseCookies();
    void parseContentLength();
    void parseTransferEncoding();
    void parseSession();

    RET_STATE parseChunked();

private:
    HttpSessionManager *sessionManager_;
    TcpConnection::ptr conn_;
    Buffer::ptr data_;

    CHECK_STATE state_;
    RET_STATE retState_;

    // 记录上一次数据解析结束的位置
    size_t lastEnd_;

    const size_t max_requestLine_len_;
    const size_t max_requestHeader_len_;

    HttpMethod method_;
    HttpUrl reqUrl_;
    HttpVersion version_;

    HttpHeader header_;
    HttpCookie cookie_;
    HttpMultiPart multipart_;
    std::string body_;
    HttpForm form_;

    HttpSession::ptr session_;

    HttpContentType contentType_;
    size_t contentLength_;
    std::string acceptEncoding_;
    std::string transferEncoding_;
    bool chunked_;

    const size_t chunk_size_str_limit;
    HttpChunkedState cur_chunkedState_;
    int cur_chunk_size_;

    bool keepAlive_;
    bool compressed_;
    bool hasContentType_;
    bool hasContentLength_;
    bool hasCookies_;
    bool hasTransferEncoding_;
};

}


#endif