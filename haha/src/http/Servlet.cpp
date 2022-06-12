#include "http/Servlet.h"
#include <fstream>
#include <filesystem>
#include <unordered_set>

namespace haha{

static const std::string BASE_ROOT = ".";
static const uintmax_t small_file_limit = 4 *1024 * 1024; // 4M

std::string Servlet::basePage(int code, const std::string &content) {
  std::string text = std::to_string(code) + " " + Status_Code.at(code);
  return R"(<html><head><title>)" + text +
         R"(</title></head><body><center><h1>)" + text +
         R"(</h1></center><hr><center>)" + content + 
         R"(</center></body></html>)";
}

ServletDispatcher::ServletDispatcher(){
    defaultServlet_ = std::make_shared<Servlet>([&](HttpRequest::ptr req, HttpResponse::ptr resp){
        auto code = resp->getStatusCode();
        if(code >= 400){
            resp->setContentType(HttpContentType::HTML);
            resp->appendBody(Servlet::basePage(code));
            return;
        }
        auto url = req->getUrl();
        auto path = url.getPath();
        bool exists = std::filesystem::exists(BASE_ROOT + std::string(path));
        if(exists){
            std::filesystem::path p(BASE_ROOT + std::string(path));
            auto ext = p.extension();
            std::unordered_set<std::string> accpetable_exts = {
                "html", "htm", "txt", "jpg", "png"
            };
            if(accpetable_exts.find(ext) != accpetable_exts.end()){
                auto fsz = std::filesystem::file_size(p);
                if(fsz <= small_file_limit){
                    resp->setContentType(Ext2HttpContentType.at(ext.c_str()));
                    resp->appendFile2Body(p.c_str());
                }
                else{
                    resp->setContentType(HttpContentType::HTML);
                    // resp->appendBody(Servlet::basePage(code, "file too big!!!"));
                    resp->setFileStream(p.c_str());
                }
            }
        }
        else{
            resp->setStatusCode(HttpStatus::NOT_FOUND);
            resp->setContentType(HttpContentType::HTML);
            resp->appendBody(Servlet::basePage(code));
            return;
        }
    });
}

void ServletDispatcher::addServlet(const std::string &uri, Servlet::servletFunc func){
    dispatcher_[uri] = std::make_shared<Servlet>(std::move(func));
}

void ServletDispatcher::addServlet(const std::string &uri, Servlet::ptr servlet){
    dispatcher_[uri] = servlet;
}

Servlet::ptr ServletDispatcher::getServlet(const std::string &uri){
    auto it = dispatcher_.find(uri);
    if(it == dispatcher_.end()){
        return defaultServlet_;
    }
    return it->second;
}

void ServletDispatcher::dispatch(HttpRequest::ptr req, HttpResponse::ptr resp, 
                                    Servlet::servletFunc preprocess, Servlet::servletFunc postprocess){
    preprocess(req, resp);

    std::string path = BASE_ROOT + std::string(req->getUrl().getPath());
    Servlet::ptr p = nullptr;
    {
        ReadWriteLock::RallReadLock lock(mutex_);
        auto it = find_match(path);
        if(it == dispatcher_.end()){
            p = defaultServlet_;
        }
        else{
            p = it->second;
        }
    }
    p->handle(req, resp);

    postprocess(req, resp);
}

ServletDispatcher::Dispatcher::iterator ServletDispatcher::find_match(const std::string& uri){
    auto it = dispatcher_.find(uri);
    if(it != dispatcher_.end()){
        return it;
    }
    for(Dispatcher::iterator it = dispatcher_.begin();
        it != dispatcher_.end();
        ++it){
        if(!::fnmatch(it->first.c_str(), uri.c_str(), 0)){
            return it;
        }
    }
    return dispatcher_.end();
}

}