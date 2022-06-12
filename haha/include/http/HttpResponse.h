#ifndef __HAHA_HTTPRESPONSE_H__
#define __HAHA_HTTPRESPONSE_H__

#include <memory>
#include <fstream>
#include <filesystem>
#include "base/TimeStamp.h"
#include "base/Buffer.h"
#include "TcpConnection.h"
#include "http/HttpUtil.h"
#include "http/HttpUrl.h"
#include "http/HttpHeader.h"
#include "http/HttpSession.h"
#include "http/HttpCookie.h"

namespace haha{

class HttpResponse{
public:
    typedef std::shared_ptr<HttpResponse> ptr;
    HttpResponse(HttpVersion, TcpConnection::ptr);
    ~HttpResponse();
    
    void setKeepAlive(bool on) { keepAlive_ = on; }
    void setVersion(HttpVersion version) { version_ = version; }
    void setStatusCode(HttpStatus statusCode) { statusCode_ = statusCode; }
    void setContentType(HttpContentType contentType) { contentType_ = contentType; }
    void setContentLength(size_t contentLength) { contentLength_ = contentLength; }
    void setCookie(const HttpCookie &cookie) {
        header_.add("Set-Cookie", cookie.toString());
    }
    void setCookie(const std::string &cookieStr){
        header_.add("Set-Cookie", cookieStr);
    }
    void addAtrribute(const std::string &key, const std::string &val){
        header_.add(key, val);
    }

    void appendBody(const std::string &data){
        buffer_->Append(data);
    }

    // 从文件读取内容，只能用于小文件读取
    void appendFile2Body(const char* file_path){
        int err;
        int file = ::open(std::filesystem::absolute(file_path).c_str(), O_RDONLY | O_NONBLOCK);
        int n;
        do{
            n = buffer_->ReadFd(file, &err);
        }while(n > 0);
        ::close(file);
    }

    void setBody(const std::string &data){
        buffer_->RetrieveAll();
        buffer_->Append(data);
    }

    HttpStatus getStatusCode() const { return statusCode_; }

    bool hasSession() const { return hasSession_; }

    void setFileStream(const char* file_path) { conn_->setFileStream(file_path); }

private:
    TcpConnection::ptr conn_;
    Buffer::ptr buffer_;
    
    HttpVersion version_;
    HttpStatus statusCode_;
    HttpHeader header_;
    HttpContentType contentType_;
    size_t contentLength_;
    bool keepAlive_;

    bool hasSession_;

    std::string file_path_;
};

}

#endif