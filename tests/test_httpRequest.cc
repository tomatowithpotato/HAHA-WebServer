#include <string_view>
#include <string>
#include <base/Buffer.h>
#include <memory>
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include <vector>
#include <random>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include <tuple>

#include "http/HttpRequest.h"
#include "http/HttpTest.h"

// void print_parseReqLine(haha::HttpRequest &req){
//     switch (req.parseRequestLine())
//     {
//     case haha::HttpRequest::OK_REQUEST:
//         std::cout << "ok" << std::endl;
//         assert(req.getBuffer()->ReadableBytes() == 0);
//         break;
//     case haha::HttpRequest::AGAIN_REQUEST:
//         std::cout << "again" << std::endl;
//         break;
//     case haha::HttpRequest::BAD_REQUEST:
//         std::cout << "bad" << std::endl;
//         break;
//     default:
//         break;
//     }
// }


// void print_parseReqHeader(haha::HttpRequest &req){
//     switch (req.parseRequestHeader())
//     {
//     case haha::HttpRequest::OK_REQUEST:
//         std::cout << "ok" << std::endl;
//         for(auto [k,v] : req.getHeader()){
//             std::cout << k << ": " << v << std::endl;
//         }
//         break;
//     case haha::HttpRequest::AGAIN_REQUEST:
//         std::cout << "again" << std::endl;
//         break;
//     case haha::HttpRequest::BAD_REQUEST:
//         std::cout << "bad" << std::endl;
//         break;
//     default:
//         break;
//     }
// }

void print_parseHttpUrl(haha::HttpUrl &url){
    std::cout << "=============HttpUrl-query=============" << std::endl;
    std::cout << url.getUrl() << std::endl;
    std::cout << url.getScheme() << std::endl;
    std::cout << url.getHost() << std::endl;
    std::cout << url.getHostName() << ":" << url.getPort() << std::endl;
    std::cout << url.getPath() << std::endl;
    for(auto [k,v] : url.getQuery()){
        std::cout << k << ": " << v << std::endl;
    }
}


void print_parseHttpCookies(haha::HttpCookie &cookies){
    std::cout << cookies.toString() << std::endl;
}

void print_parseHttpForm(haha::HttpForm &form){
    std::cout << "=============HttpForm=============" << std::endl;
    for(auto [k,v] : form){
        std::cout << k << ": " << v << std::endl;
    }
}

haha::HttpRequest::RET_STATE parseRequestBySplit(haha::HttpRequest &req, haha::Buffer::ptr buffer, const std::vector<std::string> &split){
    for(const auto &str : split){
        // std::cout << "=============step=============" << std::endl;
        buffer->Append(str);
        auto ret = req.parseRequest();
        // switch (ret)
        // {
        // case haha::HttpRequest::AGAIN_REQUEST:
        //     // std::cout << "AGAIN_REQUEST" << std::endl;
        //     break;
        // case haha::HttpRequest::BAD_REQUEST:
        //     std::cout << "BAD_REQUEST" << std::endl;
        //     return ret;
        //     break;
        // case haha::HttpRequest::OK_REQUEST:
        //     std::cout << "OK_REQUEST" << std::endl;
        //     return ret;
        //     break;
        // default:
        //     break;
        // }
    }
    return req.getRetState();
}


int main(){
    // srand(time(NULL));

    // haha::Buffer::ptr buffer = std::make_shared<haha::Buffer>();
    // buffer->Append("GET /index.html HTTP/1.1");
    // haha::HttpRequest req(buffer);
    // print_parseReqLine(req);
    // haha::Buffer::ptr buffer1 = std::make_shared<haha::Buffer>();
    // buffer1->Append("POST /yes/index.html HTTP/1.1\r\n");
    // haha::HttpRequest req1(buffer1);
    // print_parseReqLine(req1);
    // haha::Buffer::ptr buffer2 = std::make_shared<haha::Buffer>();
    // buffer2->Append("GET /index.html HTTP/1.\r\n");
    // haha::HttpRequest req2(buffer2);
    // print_parseReqLine(req2);

    // haha::Buffer::ptr buffer3 = std::make_shared<haha::Buffer>();
    // buffer3->Append("nihao: en\r\nxiexie: gun\r\n");
    // haha::HttpRequest req3(buffer3);
    // print_parseReqHeader(req3);
    // haha::Buffer::ptr buffer4 = std::make_shared<haha::Buffer>();
    // buffer4->Append("nihao: en\r\nxiexie:   gun\r\n\r\n");
    // haha::HttpRequest req4(buffer4);
    // print_parseReqHeader(req4);
    // haha::Buffer::ptr buffer5 = std::make_shared<haha::Buffer>();
    // buffer5->Append("nihao: en\r\nxiexie:1gun\r\n\r\n");
    // haha::HttpRequest req5(buffer5);
    // print_parseReqHeader(req5);

    // const char *encodedUrl = "https://translate.google.cn/?sl=zh-CN&tl=en&text=%E9%92%A6%E4%BD%A9&op=translate";
    // haha::HttpUrl url(encodedUrl);
    // print_parseHttpUrl(url);

    // const char *encodedUrl1 = "https://static.kancloud.cn/digest/understandingnginx/202605";
    // haha::HttpUrl url1(encodedUrl1);
    // print_parseHttpUrl(url1);

    // const char *encodedUrl2 = "https://192.168.1.10:8888/digest/understandingnginx/202605";
    // haha::HttpUrl url2(encodedUrl2);
    // print_parseHttpUrl(url2);

    // const char *cookies_str = "reg_fb_gate=deleted; Expires=Thu, 01 Jan 1970 00:00:01 GMT; Path=/; Domain=.example.com; HttpOnly; name=lfr";
    // haha::HttpCookie cookies(cookies_str);
    // print_parseHttpCookies(cookies);

    // const char *formstr = "key1=dddd&key2=%E9%92%A6%E4%BD%A9&key3=%A3%B4%C8&key4=ha%B7&hfdhgsdsaf";
    // haha::HttpForm form;
    // haha::parseKeyValue(formstr, "=", "&", form);
    // print_parseHttpForm(form);

    // std::cout << "==============================randomSplitStr==============================" << std::endl;
    // std::string s("1234567891011121314151617181920");
    // auto splits = haha::test::randomSplitStr(s+s+s, 10);
    // for(auto splt : splits){
    //     std::cout << splt << std::endl;
    // }

    for(int i = 0; i < 100; ++i){
        std::cout << "=============New Loop=============" << std::endl;
        haha::test::RandomHttpRequestString httpRequstStr;
        std::cout << "==============================Line==============================" << std::endl;
        std::cout << httpRequstStr.getRequestLine() << std::endl;
        std::cout << "==============================Header==============================" << std::endl;
        std::cout << httpRequstStr.getRequestHeader() << std::endl;
        std::cout << "==============================Body==============================" << std::endl;
        std::cout << httpRequstStr.getRequestBody() << std::endl;
        haha::Buffer::ptr buffer = std::make_shared<haha::Buffer>();
        haha::HttpRequest req(nullptr, buffer);
        for(int j = 0; j < 250; ++j){
            auto splits = haha::test::randomSplitStr(httpRequstStr.getRequest(), 10);
            parseRequestBySplit(req, buffer, splits);
            switch (req.getState())
            {
            case haha::HttpRequest::CHECK_REQUESTLINE:
                std::cout << "CHECK_REQUESTLINE" << std::endl;
                break;
            case haha::HttpRequest::CHECK_HEADER:
                std::cout << "CHECK_HEADER" << std::endl;
                for(auto [k, v] : req.getHeader()){
                    std::cout << k << ": " << v << std::endl;
                }
                break;
            case haha::HttpRequest::CHECK_CONTENT:
                std::cout << "CHECK_CONTENT" << std::endl;
                std::cout << req.getBody() << std::endl;
                break;
            case haha::HttpRequest::CHECK_DONE:
                // std::cout << "CHECK_DONE" << std::endl;
                break;
            default:
                break;
            }
            assert(req.getState() == haha::HttpRequest::CHECK_DONE);
        }
    }

    return 0;
}