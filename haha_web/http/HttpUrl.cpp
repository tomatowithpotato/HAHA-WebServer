#include "http/HttpUrl.h"

namespace haha{

/* ************************************************HTTP URL************************************************ */

HttpUrl::HttpUrl(const std::string& url, bool decode){
    if(decode){
        url_ = EncodeUtil::urlDecode(url);
    }
    if(!url.empty() && url_.empty()){
        state_ = PARSE_FAIL;
    }
    else{
        state_ = parseFromUrl();
    }
}

HttpUrl::STATE HttpUrl::parseFromUrl(){
    // size_t n = url_.size();
    std::string_view url_view(url_);
    size_t pos;

    if(url_view.compare(0, 4, "http")){
        // 有host
        // 解析scheme
        pos = url_view.find("://");
        if(pos == url_view.npos){
            return PARSE_FAIL;
        }
        std::string_view scheme_strview = url_view.substr(0, pos);
        if(scheme_strview == "http"){
            scheme_ = HTTP;
        }
        else if(scheme_strview == "https"){
            scheme_ = HTTPS;
        }
        else{
            return PARSE_FAIL;
        }

        // 解析host
        url_view.remove_prefix(pos+3);
        if(url_view.size() < 2){
            return PARSE_FAIL;
        }
        pos = url_view.find("/");
        if(pos == url_view.npos){
            host_ = std::string(url_view);
            hostName_ = host_;
            path_ = "/";
            return PARSE_SUCCESS;
        }
        else{
            host_ = std::string(url_view.substr(0, pos));
            size_t pos1 = host_.find(":");
            if(pos1 != host_.npos){
                hostName_ = host_.substr(0, pos1);
                port_ = host_.substr(pos1+1);
            }
            else{
                hostName_ = host_;
            }
        }
        url_view.remove_prefix(pos); // '/'要保留
    }
    else{
        // 没host
        if(!url_view.compare(0, 1, "/")){
            return PARSE_FAIL;
        }
    }

    // 解析path
    pos = url_view.find('?');
    if(pos == url_view.npos){
        path_ = std::string(url_view);
        return PARSE_SUCCESS;
    }
    else{
        path_ = std::string(url_view.substr(0, pos));
    }

    // 解析query
    url_view.remove_prefix(pos+1);
    std::string_view query_strview = url_view;
    std::string_view key;
    std::string_view val;
    size_t j = 0;
    size_t i = 0;
    for(i = 0; i < query_strview.size(); ++i){
        if(query_strview[i] == '='){
            key = query_strview.substr(j, i-j);
            j = i+1;
        }
        else if(query_strview[i] == '&'){
            val = query_strview.substr(j, i-j);
            j = i+1;
            query_[std::string(key)] = std::string(val);
            key = "";
            val = "";
        }
    }
    if(!key.empty()){
        val = query_strview.substr(j, i-j);
        query_[std::string(key)] = std::string(val);
    }

    return PARSE_SUCCESS;
}

}