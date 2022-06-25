#include "http/HttpServer.h"

namespace haha{

HttpServer::HttpServer()
    :sessionManger_(&HttpSessionManager::getInstance())
    ,servletDispatcher_(std::make_shared<ServletDispatcher>()){
    
}

HttpServer::MESSAGE_STATUS HttpServer::onMessage(TcpConnection::ptr conn){
    std::shared_ptr<HttpRequest> request;
    if(conn->getContext() == nullptr){
        request = std::make_shared<HttpRequest>(sessionManger_, conn);
        conn->setContext(request);
    }
    else{
        request = std::static_pointer_cast<HttpRequest>(conn->getContext());
    }

    auto ret = request->parseRequest();

    HttpResponse::ptr response = std::make_shared<HttpResponse>(request->getVersion(), conn);

    switch (ret)
    {
    case haha::HttpRequest::RET_STATE::AGAIN_REQUEST:
        return MESSAGE_STATUS::AGAIN;
        break;
    case haha::HttpRequest::RET_STATE::OK_REQUEST:
        /* code */ 
        break;
    case haha::HttpRequest::RET_STATE::BAD_REQUEST:
        conn->setKeepAlive(false);
        response->setStatusCode(HttpStatus::BAD_REQUEST);
        // response->addAtrribute("Connection", "close");
        response->setKeepAlive(false);
        break;
    default:
        break;
    }

    if(servletDispatcher_){
        servletDispatcher_->dispatch(request, response, 
            [&](HttpRequest::ptr req, HttpResponse::ptr resp){
                beforeServlet(req, resp);
            },
            [&](HttpRequest::ptr req, HttpResponse::ptr resp){
                afterServlet(req, resp);
            });
    }

    conn->setContext(nullptr);

    return HttpServer::MESSAGE_STATUS::OK;
}

bool HttpServer::onCreateConnection(TcpConnection::ptr conn){
    return true;
}

bool HttpServer::onCloseConntection(TcpConnection::ptr conn){
    return true;
}

void HttpServer::beforeServlet(HttpRequest::ptr req, HttpResponse::ptr resp){
    auto req_ssid = req->getCookie().getSessionId();
    if(!req_ssid.empty()){
        HttpCookie cookie;
        cookie.add("SESSIONID", req_ssid);
        resp->setCookie(cookie);
    }
}

void HttpServer::afterServlet(HttpRequest::ptr req, HttpResponse::ptr resp){
    auto session = req->getSession(false);
    if(session){
        // The old session has been set to an invalid state, so it is
        // automatically deleted by the timer, and a null pointer object is
        // returned
        if(!session->isDestroy()){
            if(session->isNew()){
                session->setStatus(HttpSession::Accessed);
                getMainLoop()->addTimer(
                    Timer(
                        EncodeUtil::murmurHash2(session->getId()),
                        TimeStamp::nowSecond(session->getMaxInactiveInterval()),
                        [&](){handleSessionTimeout(session);}
                    ));
            }
            else{
                getMainLoop()->adjustTimer(
                    Timer(
                        EncodeUtil::murmurHash2(session->getId()),
                        TimeStamp::nowSecond(session->getMaxInactiveInterval()),
                        nullptr
                    ));
            }
        }
    }
}

void HttpServer::handleSessionTimeout(HttpSession::ptr session){
    if(session){
        session->setStatus(HttpSession::Destroy);
        sessionManger_->delSession(session->getId());
    }
}

}