#include "http/HttpResponse.h"

namespace haha{

HttpResponse::HttpResponse(HttpVersion version, TcpConnection::ptr conn)
    :conn_(conn)
    ,version_(version)
    ,statusCode_(HttpStatus::OK)
    ,contentLength_(-1)
    ,keepAlive_(false)
    ,hasSession_(false)
    ,isFileBody_(false)
{
    buffer_ = conn_->getSender();
    header_.add("Server", "HAHA_WEBSERVER");
    header_.add("Date", TimeStamp::getDate());
    header_.setContentType("application/octet-stream");
}

HttpResponse::~HttpResponse(){
    // 响应首行
    buffer_->Append(std::string(HttpVersion2Str.at(version_)) + 
                    " " + std::to_string(statusCode_) + " "
                    + HttpStatus2Str.at(statusCode_) + "\r\n");
    if(contentLength_ == -1){
        if(isFileBody_){
            contentLength_ = std::filesystem::file_size(file_path_);
        }
        else{
            contentLength_ = body_.size();
        }
        header_.setContentLength(std::to_string(contentLength_));
    }

    // 响应头
    for(const auto &[k, v] : header_){
        buffer_->Append(k + ": " + v + "\r\n");
    }
    buffer_->Append("\r\n");

    if(!isFileBody_){
        // 普通内容，填入buffer中
        buffer_->Append(body_);
    }
}

}