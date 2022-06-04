#ifndef __HAHA_HTTPURL_H__
#define __HAHA_HTTPURL_H__

#include <string>
#include <string_view>
#include <unordered_map>
#include "http/HttpUtil.h"

namespace haha{

class HttpUrl{
public:
    typedef std::unordered_map<std::string_view, std::string_view> Query;
    enum STATE{
        PARSE_SUCCESS,
        PARSE_FAIL,
    };
    enum SCHEME{
        HTTP,
        HTTPS,
    };
    HttpUrl(std::string_view url = "", bool decode = true);
    bool empty(){return url_.empty();}
    size_t size(){return url_.size();}

    STATE getState() { return state_; }

    std::string_view getUrl(){ return std::string_view(url_.c_str()); }

    std::string_view getScheme() { 
        switch (scheme_)
        {
        case HTTP:
            return "HTTP";
            break;
        case HTTPS:
            return "HTTPS";
            break;
        default:
            return "";
            break;
        }
    }
    std::string_view getHost() { return host_; }
    std::string_view getHostName() { return hostName_; }
    std::string_view getPort() { return port_; }
    std::string_view getPath() { return path_; }
    Query getQuery() { return query_; }

private:
    STATE parseFromUrl();

private:
    STATE state_;
    std::string url_;
    SCHEME scheme_;
    std::string_view host_;
    std::string_view hostName_;
    std::string_view port_;
    std::string_view path_;
    Query query_;
};

}


#endif