#ifndef __HAHA_HTTPSERVER_H__
#define __HAHA_HTTPSERVER_H__

#include <memory>
#include "net/TcpServer.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "http/HttpSession.h"
#include "http/Servlet.h"
#include "base/EncodeUtil.h"
#include "base/TimeStamp.h"

namespace haha{

class HttpServer : public TcpServer{
public:
    HttpServer();
    void setServletDispatcher(ServletDispatcher::ptr servletDispatcher){
        servletDispatcher_ = servletDispatcher;
    }
    void addServlet(const std::string &uri, Servlet::servletFunc func){
        servletDispatcher_->addServlet(uri, std::move(func));
    }
    void addServlet(const std::string &uri, Servlet::ptr servlet){
        servletDispatcher_->addServlet(uri, servlet);
    }

protected:
    MESSAGE_STATUS onMessage(TcpConnection::ptr) override;
    bool onCreateConnection(TcpConnection::ptr) override;
    bool onCloseConntection(TcpConnection::ptr) override;

    void beforeServlet(HttpRequest::ptr, HttpResponse::ptr);
    void afterServlet(HttpRequest::ptr, HttpResponse::ptr);

    void handleSessionTimeout(HttpSession::ptr session);

private:
    HttpSessionManager *sessionManger_;
    ServletDispatcher::ptr servletDispatcher_;
};

}

#endif