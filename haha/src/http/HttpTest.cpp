#include "http/HttpTest.h"

namespace haha
{
    
namespace test{

std::string genRandomStr(size_t n) {
    std::string res;
    res.reserve(n);
    static constexpr const char *alpha =
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \r\n*~&;=";
    for(size_t i = 0; i < n; ++i)
        res.push_back(alpha[::rand() % 62]);
    return res;
}

std::string genRandomKVStr(size_t n, const std::string &join, const std::string &split, bool urlencode){
    assert(n >= join.size() + split.size() + 2);
    n -= join.size() + split.size();
    size_t n1 = rand() % (n-1) + 1;
    size_t n2 = n - n1;
    std::string k = genRandomStr(n1);
    std::string v = genRandomStr(n2);
    if(urlencode){
        k = EncodeUtil::urlEncode(k);
        v = EncodeUtil::urlEncode(v);
    }
    return k + join + v + split;
}

std::string genRandomKVStrs(size_t n, const std::string &join, const std::string &split, bool urlencode){
    size_t min_kv_sz = join.size() + split.size() + 2;
    assert(n >= min_kv_sz);
    std::string res;
    size_t i = n;
    while(i){
        size_t kv_sz;
        if(i < min_kv_sz * 2){
            kv_sz = i;
        }
        else{
            kv_sz = std::min(std::max(rand() % i + 1, min_kv_sz), i - min_kv_sz);
        }
        std::string kv = genRandomKVStr(kv_sz, join, split, urlencode);
        i -= kv_sz;
        res += kv;
    }
    return res;
}


std::vector<std::string> randomSplitStr(const std::string &str, size_t limit){
    ::srand(::time(NULL));
    std::string_view view(str);
    std::vector<std::string> res;
    size_t n = str.size();
    while(view.size()){
        size_t i = rand() % std::min(view.size(), limit) + 1;
        res.push_back(std::string(view.substr(0, i)));
        view.remove_prefix(i);
    }
    return res;
}


RandomHttpRequestString::RandomHttpRequestString(){
    ::srand(::time(NULL));
    auto ret = create_random_RequestBody();
    requestBody_ = std::get<0>(ret);
    contentType_ = std::get<1>(ret);
    chunked_ = std::get<2>(ret);
    requestHeader_ = create_random_RequestHeader(requestBody_, contentType_, chunked_);
    requestLine_ = create_random_RequestLine();
    request_ = requestLine_ + requestHeader_ + requestBody_;
}

std::string RandomHttpRequestString::create_random_RequestLine(){
    std::vector<std::string> methods = {"GET","POST","HEAD"};
    std::string method = methods[::rand() % methods.size()];

    std::vector<std::string> schemes = {"http", "https"};
    std::vector<std::string> hostnames = {"baidu","google","taobao"};
    std::vector<std::string> ports = {"8080"};
    std::vector<std::string> paths = {"/", "/file", "/search", "can/can/need"};
    std::vector<std::string> querys = {
        "sl=zh-CN&tl=en&text=%E9%92%A6%E4%BD%A9&op=translate",
        "q=c%2B%2B11+随机数&newwindow=1&sxsrf=ALiCzsbwl4fBbYQ780AY5kSKXzsHro4EJQ%3A1654172383021",
        "wd=年后&rsv_spt=1&rsv_iqid=0xa99b55fa00000d7b",
    };
    
    std::string url;
    url += schemes[::rand() % schemes.size()] + "://";
    url += "www." + hostnames[::rand() % hostnames.size()] + ".com";
    url += rand() % 2 ? ":" + ports[::rand() % ports.size()] : "";
    url += paths[::rand() % paths.size()];
    url += "?" + querys[::rand() % querys.size()];

    std::vector<std::string> versions = {"HTTP/1.0","HTTP/1.1","HTTP/2.0"};
    std::string version = versions[::rand() % versions.size()];

    std::string requestLine = method + " " + url + " " + version + "\r\n";

    return requestLine;
}


std::string RandomHttpRequestString::create_random_RequestHeader(const std::string &requestBody, const std::string &contentType, bool chunked){
    std::vector<std::string> fields = {
        "Content-Length",
        "Transfer-Encoding",
        "Accept-Encoding",
        "Content-Type",
        "Connection",
        "Cookie",
    };

    if(!chunked){
        fields.erase(fields.begin()+1);
    }

    std::unordered_map<std::string, std::vector<std::string>> fields_map = {
        {"Content-Length", {std::to_string(requestBody.size())}},
        {"Transfer-Encoding", {"chunked"}},
        {"Accept-Encoding", {"gzip"}},
        {"Content-Type", {contentType}},
        {"Connection", {"Close", "Keep-Alive", "keep-Alive", "close"}},
        {"Cookie", {
            "reg_fb_gate=deleted; key3=14; Expires=Thu, 01 Jan 1970 00:00:01 GMT; Path=/; Domain=.example.com; HttpOnly; name=lfr",
            "BAIDU_SSP_lcr=https://www.google.com.hk/; uuid_tt_dd=10_21113699420-1615529077584-850075; UN=oto222333;",
            "prov=eb25da02-1e8f-5345-e936-4c70b050a938; _ga=GA1.2.361922288.1617852921; OptanonAlertBoxClosed=2021-06-06T01:31:51.310Z; __qca=P0-1960564160-1622946794364",
        }},
        {"Funny", {"joy","boy"}},
    };

    std::random_shuffle(fields.begin(), fields.end());

    std::string header;
    for(const auto &k : fields){
        auto v = fields_map[k];
        header += k + ": " + v[::rand() % v.size()] + "\r\n";
    }

    header += "\r\n";

    return header;
}


std::tuple<std::string, std::string, bool> RandomHttpRequestString::create_random_RequestBody(){
    std::string body;
    std::string content_type;

    bool chunked = rand() % 2;

    size_t ct = rand() % 5;
    switch (ct)
    {
    case 0:
        content_type = "application/x-www-form-urlencoded";
        break;
    case 1:
        content_type = "text/plain";
    // case 2:
    //     content_type = "multipart/form-data";
    default:
        content_type = "text/plain";
        break;
    }

    if(chunked){
        size_t min_ck_sz = 4;
        size_t n = rand() % 5 + 1;
        while(n--){
            size_t ck_sz = std::max(int(min_ck_sz), rand() % 50 + 1);
            std::string chunk;
            if(content_type == "application/x-www-form-urlencoded"){
                chunk = genRandomKVStrs(ck_sz,"=","&", true);
            }
            else if(content_type == "text/plain"){
                chunk = genRandomKVStrs(ck_sz,"=","&");
                /* do nothing */
            }
            // else if(content_type == "multipart/form-data"){

            // }
            body += haha::int2HexString(ck_sz) + "\r\n";
            body += chunk + "\r\n";
        }
        body += "0\r\n";
        body += "\r\n";
    }
    else{
        if(content_type == "application/x-www-form-urlencoded"){
            body = genRandomKVStrs(std::max(4, rand() % 150),"=","&", true);
        }
        else if(content_type == "text/plain"){
            body = genRandomKVStrs(std::max(4, rand() % 150),"=","&");
            /* do nothing */
        }
        // else if(content_type == "multipart/form-data"){

        // }
    }

    return {body, content_type, chunked};
}

}

} // namespace haha
