#include "http/HttpResponse.h"
#include "magic_enum/magic_enum.hpp"

namespace haha{

HttpResponse::HttpResponse(HttpVersion version, TcpConnection::ptr conn)
    :version_(version)
    ,statusCode_(HttpStatus::OK)
    ,conn_(conn)
    ,hasSession_(false)
    ,keepAlive_(false)
    ,contentLength_(-1)
    ,isFileBody_(false){
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
    // 填入文件内容（如果有的话）
    if(isFileBody_){
        int err;
        int file = ::open(std::filesystem::absolute(file_path_).c_str(), O_RDONLY | O_NONBLOCK);
        int n;
        do{
            n = buffer_->ReadFd(file, &err);
        }while(n > 0);
        ::close(file);
    }
    // 普通内容
    else{
        buffer_->Append(body_);
    }
}

}