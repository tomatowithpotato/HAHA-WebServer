#include "http/Servlet.h"
#include <fstream>
#include <filesystem>
#include <unordered_set>

namespace haha{

static const std::string BASE_ROOT = "./resource";
static const uintmax_t small_file_limit = 4 *1024 * 1024; // 4M
// static const uintmax_t small_file_limit = 0;

std::string Servlet::basePage(int code, const std::string &content) {
  std::string text = std::to_string(code) + " " + HttpStatus2Str.at(code);
  return R"(<html><head><title>)" + text +
         R"(</title></head><body><center><h1>)" + text +
         R"(</h1></center><hr><center>)" + content + 
         R"(</center></body></html>)";
}

ServletDispatcher::ServletDispatcher(){
    defaultServlet_ = std::make_shared<Servlet>([&](HttpRequest::ptr req, HttpResponse::ptr resp){
        auto &reqBody = req->getBody();
        /* 暂时没想好怎么处理请求体的数据，先不管 */

        auto code = resp->getStatusCode();
        if(code >= 400){
            resp->setContentType(HttpContentType::HTML);
            resp->appendBody(Servlet::basePage(code));
            return;
        }

        auto url = req->getUrl();
        std::string path(url.getPath());
        if(path == "/"){
            path = "/index.html";
        }
        // char buf[1024];
        // getcwd(buf, sizeof(buf));
        // std::filesystem::path cur_path = std::filesystem::current_path();
        std::filesystem::path p(BASE_ROOT + std::string(path));
        bool exists = std::filesystem::exists(p);
        if(exists){
            std::string ext;
            if(p.has_extension()){
                ext = std::string(p.extension().c_str());
            }
            std::unordered_set<std::string> accpetable_exts = {
                ".html", ".htm", ".txt", ".jpg", ".png"
            };
            if(accpetable_exts.find(ext) != accpetable_exts.end()){
                auto fsz = std::filesystem::file_size(p);
                // if(fsz <= small_file_limit){
                //     // 小文件
                //     resp->setContentType(Ext2HttpContentType.at(ext));
                //     resp->setFileBody(p.c_str());
                // }
                // else{
                //     // 大文件
                //     resp->setContentType(HttpContentType::HTML);
                //     resp->setFileStream(p.c_str());
                // }
                resp->setContentType(Ext2HttpContentType.at(ext));
                resp->setFileStream(p.c_str());
            }
            else{
                resp->setContentType(HttpContentType::HTML);
                resp->appendBody(Servlet::basePage(code, "I don't know what do you really want"));
            }
        }
        else{
            resp->setStatusCode(HttpStatus::NOT_FOUND);
            resp->setContentType(HttpContentType::HTML);
            resp->appendBody(Servlet::basePage(HttpStatus::NOT_FOUND));
            return;
        }
    });
}

void ServletDispatcher::addServlet(const std::string &uri, Servlet::servletFunc func){
    ReadWriteLock::RAIIWriteLock lock(mutex_);
    dispatcher_[uri] = std::make_shared<Servlet>(std::move(func));
}

void ServletDispatcher::addServlet(const std::string &uri, Servlet::ptr servlet){
    ReadWriteLock::RAIIWriteLock lock(mutex_);
    dispatcher_[uri] = servlet;
}

Servlet::ptr ServletDispatcher::getServlet(const std::string &uri){
    ReadWriteLock::RAIIReadLock lock(mutex_);
    auto it = dispatcher_.find(uri);
    if(it == dispatcher_.end()){
        return defaultServlet_;
    }
    return it->second;
}

void ServletDispatcher::dispatch(HttpRequest::ptr req, HttpResponse::ptr resp, 
                                    Servlet::servletFunc preprocess, Servlet::servletFunc postprocess){
    preprocess(req, resp);

    std::string path(req->getUrl().getPath());
    Servlet::ptr p = nullptr;
    
    auto it = find_match(path);
    if(it == dispatcher_.end()){
        p = defaultServlet_;
    }
    else{
        p = it->second;
    }
    
    p->handle(req, resp);

    postprocess(req, resp);
}

ServletDispatcher::Dispatcher::iterator ServletDispatcher::find_match(const std::string& uri){
    ReadWriteLock::RAIIReadLock lock(mutex_);
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