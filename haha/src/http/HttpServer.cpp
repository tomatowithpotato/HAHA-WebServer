#include "http/HttpServer.h"

namespace haha{

HttpServer::MESSAGE_STATUS HttpServer::onMessage(TcpConnection::ptr conn){
    std::shared_ptr<HttpRequest> request;
    if(conn->getContext() == nullptr){
        request = std::make_shared<HttpRequest>();
        conn->setContext(request);
    }
    else{
        request = std::static_pointer_cast<HttpRequest>(conn->getContext());
    }

    if(conn->isDisconnected()){
        return MESSAGE_STATUS::CLOSE;
    }

    auto ret = request->parseRequest();

    switch (ret)
    {
    case haha::HttpRequest::RET_STATE::AGAIN_REQUEST:
        return MESSAGE_STATUS::AGAIN;
        break;
    case haha::HttpRequest::RET_STATE::OK_REQUEST:
        /* code */ 
        break;
    case haha::HttpRequest::RET_STATE::BAD_REQUEST:
        return MESSAGE_STATUS::CLOSE;
        break;
    default:
        break;
    }

    return HttpServer::MESSAGE_STATUS::OK;
}

bool HttpServer::onNewConntection(TcpConnection::ptr conn){
    return true;
}

bool HttpServer::onCloseConntection(TcpConnection::ptr conn){
    return true;
}
}