# HAHAWebServer

一个用c++20实现的简易http服务器，还在更新中

## 技术特点：

    1. 基于epoll + 阻塞队列 + 线程池

    2. 采用时间堆管理超时连接

    3. 利用智能指针进行内存的管理，最大限度避免内存泄漏

    4. 利用mmap或sendfile进行文件传输

    5. 较简陋的日志系统

## 目前支持如下功能：

    1. 支持http1.0、http1.1协议

    2. 支持cookie和session

    3. 支持文件的上传和下载（还未全部完成）

    3. 提供servlet编程接口

## 未来的计划：

    1. 加入配置系统

    2. 优化日志系统，改为异步模式

    3. 添加对https的支持

    4. 将数据存储进mysql，比如session，用户登录的账号和密码等

    5. 将阻塞队列 + 线程池模型改进为one loop per thread

    6. 实现更完善的http协议

    7. 实现负载均衡功能

    8. 实现诸如聊天室、email、游戏服务器等附带功能


## 快速使用：

编译
```shell
cd 当前目录
./build_release.sh
```
运行
```shell
cd 当前目录
./run_httpServer_release.sh
```

测试：

    运行后，打开浏览器，输入http://你的ip地址:9999/
    例如 http://0.0.0.0:9999/dog.html


## 压力测试

测试环境是我电脑的虚拟机，
cpu为i5-10400F
虚拟机内存4G
分配6个核心

单reactor模型在分支single_reactor中

"one loop per thread"模型在分支"one_loop_per_thread"中

以下分别对nginx、TinyWebServer和HAHA-WebServer做压力测试
使用TinyWebServer中自带的Webbench进行测试
没有提到的地方都采用默认配置

HAHA和Tiny每次响应的页面数据量基本接近（TinyWebServer对默认的webbench请求返回的是404页面，跟HAHA的默认返回页面差不多大），因而可以排除页面大小干扰

用webbench对nginx进行压力测试的结果
![img1](./resource/nginx-webbench-5000-5.png)

用webbench对TinyWebServer进行压力测试的结果，关闭日志，LT+ET模式，不开启编译优化
![img2](./resource/tiny-lt%2Bet-debug-webbench-5000-5.png)

用webbench对TinyWebServer进行压力测试的结果，关闭日志，LT+ET模式，开启O2级别编译优化
![img3](./resource/tiny-lt%2Bet-release-webbench-5000-5.png)

用webbench对HAHA-WebServer进行压力测试的结果，单reactor搭配阻塞队列模型，不开启编译优化
![img4](./resource/haha-debug-webbench-5000-5.png)

用webbench对HAHA-WebServer进行压力测试的结果，单reactor搭配阻塞队列模型，开启O2级别编译优化
![img5](./resource/haha-release-webbench-5000-5.png)

用webbench对HAHA-WebServer进行压力测试的结果，"one loop per thread" 模型，不开启编译优化
![img5](./resource/haha-olpt-debug-webbench-5000-5.png)

用webbench对HAHA-WebServer进行压力测试的结果，"one loop per thread" 模型，开启O2级别编译优化
![img6](./resource/haha-olpt-release-webbench-5000-5.png)

通过上述结果，可得到如下信息:

    1. nginx碾压后两者，而且是在每次相应的数据要多于前两者的情况下

    2. 不开编译优化的话，HAHA处理的请求数少于TinyWebServer，但实际上处理的数据多于Tiny（因为页面数据更大）

    3. 开启编译优化后，HAHA性能提升了数倍，远远好于TinyWebServer
    
    4. TinyWebServer开启O2级别编译优化后，性能并没有什么提升，不知是为何

    5. 采用one loop per thread模型，性能得到了一点提升，但不是很大，可能的原因：

        1. 并发量还不够大，只有5000，无法体现优势，以后换更好的机子测

        2. 数据读写处理有待优化

        3. 可以尝试用03级别优化看看
        
        4. 代码写得有问题，跟muduo原版写法还有很多差距


## Servlet使用

以下是servlet使用的小例子，代码在tests文件夹下的test_servlet.cc
```c++
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
    // auto &reqBody = req->getBody();
    // // 如果请求中有session，就会返回一个已有的
    // // 如果没有或者是过期的，那就会创建一个新的session
    // haha::HttpSession::ptr session = req->getSession();
    // // 我还没怎么测过session，可能有bug

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
            // auto fsz = std::filesystem::file_size(p);
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
            haha::HttpCookie cookie;
            cookie.add("liming", "hi");
            resp->setCookie(cookie);
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
```