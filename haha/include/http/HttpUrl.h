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
    
    bool empty() const {return url_.empty();}
    size_t size() const { return url_.size(); }

    STATE getState() const { return state_; }

    const std::string& getUrl() const { return url_; }

    std::string_view getScheme() const { 
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
    std::string_view getHost() const { return host_; }
    std::string_view getHostName() const { return hostName_; }
    std::string_view getPort() const { return port_; }
    std::string_view getPath() const { return path_; }
    const Query& getQuery() const { return query_; }

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