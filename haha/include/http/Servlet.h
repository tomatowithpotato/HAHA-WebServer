#ifndef __HAHA_SERVLET_H__
#define __HAHA_SERVLET_H__

#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <http/HttpRequest.h>
#include <http/HttpResponse.h>
#include <fnmatch.h>
#include "base/ReadWriteLock.h"

namespace haha{

class Servlet{
public:
    typedef std::shared_ptr<Servlet> ptr;
    typedef std::function<void(HttpRequest::ptr, HttpResponse::ptr)> servletFunc;
    Servlet(servletFunc func):func_(func){}
    void handle(HttpRequest::ptr request, HttpResponse::ptr response) {
        func_(request, response);
    }
    static std::string basePage(int code, const std::string &content = "HAHA");
private:
    servletFunc func_;
};

class ServletDispatcher{
public:
    ServletDispatcher();
    typedef std::shared_ptr<ServletDispatcher> ptr;
    typedef std::unordered_map<std::string, Servlet::ptr> Dispatcher;
    void addServlet(const std::string &uri, Servlet::servletFunc);
    void addServlet(const std::string &uri, Servlet::ptr);
    Servlet::ptr getServlet(const std::string &uri);
    // 将请求分发到对应的servlet
    void dispatch(HttpRequest::ptr, HttpResponse::ptr, Servlet::servletFunc preprocess, Servlet::servletFunc postprocess);

private:
    // 找到匹配的servlet
    Dispatcher::iterator find_match(const std::string& uri);

private:
    Dispatcher dispatcher_;
    Servlet::ptr defaultServlet_;
    ReadWriteLock mutex_;
};

}

#endif