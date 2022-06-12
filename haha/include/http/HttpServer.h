#ifndef __HAHA_HTTPSERVER_H__
#define __HAHA_HTTPSERVER_H__

#include <memory>
#include "TcpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpSession.h"
#include "Servlet.h"
#include "base/EncodeUtil.h"
#include "base/TimeStamp.h"

namespace haha{

class HttpServer : public TcpServer{
public:
    HttpServer():servletDispatcher_(std::make_shared<ServletDispatcher>()){}
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
    bool onNewConntection(TcpConnection::ptr) override;
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