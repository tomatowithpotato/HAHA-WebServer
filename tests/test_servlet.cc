#include <string>
#include <unordered_set>
#include "http/Servlet.h"
#include "http/HttpServer.h"

// 工作目录为可执行文件所在目录，即bin目录
// bin目录下有个resource目录，建议把你要加的html、图片之类的放那里边
static const std::string BASE_ROOT = "./resource";

// 这里只是个示例，与默认处理近似，可自行修改
// 但大体流程不建议变动
void watch_dog(haha::HttpRequest::ptr req, haha::HttpResponse::ptr resp){
    auto &reqBody = req->getBody();
    // 暂时没想好怎么处理请求体的数据，先不管
    // 请自由发挥

    auto code = resp->getStatusCode();
    // 出错，返回错误页面
    if(code >= 400){
        resp->setContentType(haha::HttpContentType::HTML);
        resp->appendBody(haha::Servlet::basePage(code));
        return;
    }

    std::string path = "/dog.html";

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
            // // setFileBody是直接用read读取文件内容到缓冲区然后再write发出去
            // // 这个效率不高，但如果你想你可以用这个，虽然我不知道你为什么会想
            // // 最好使用setFileStream
            // // 二者不可以混用！！！
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
            resp->setContentType(haha::Ext2HttpContentType.at(ext));
            resp->setFileStream(p.c_str());
        }
        else{
            resp->setContentType(haha::HttpContentType::HTML);
            resp->appendBody(haha::Servlet::basePage(code, "I don't know what do you really want"));
        }
    }
    else{
        resp->setStatusCode(haha::HttpStatus::NOT_FOUND);
        resp->setContentType(haha::HttpContentType::HTML);
        resp->appendBody(haha::Servlet::basePage(haha::HttpStatus::NOT_FOUND));
        return;
    }
}


int main(){
    // 这里的9999是端口号，可自行修改
    haha::InetAddress address(9999);
    haha::HttpServer server;
    // 添加servlet到服务器，这边我把两种请求都映射到了watch_dog上
    server.addServlet("/dog", [](haha::HttpRequest::ptr req, haha::HttpResponse::ptr resp){
        watch_dog(req, resp);
    });
    server.addServlet("/watch_dog", [](haha::HttpRequest::ptr req, haha::HttpResponse::ptr resp){
        watch_dog(req, resp);
    });

    server.start(address);
    return 0;
}