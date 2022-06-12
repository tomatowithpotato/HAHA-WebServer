#include "http/HttpResponse.h"

namespace haha{

HttpResponse::HttpResponse(HttpVersion version, TcpConnection::ptr conn)
    :version_(version)
    ,statusCode_(HttpStatus::OK)
    ,conn_(conn)
    ,hasSession_(false)
    ,keepAlive_(false){
    buffer_ = conn_->getSender();
    header_.add("Server", "HAHA_WEBSERVER");
    header_.add("Date", TimeStamp::getDate());
    header_.add("Content-Type", "application/octet-stream");
}

HttpResponse::~HttpResponse(){
    
}

}