#include "http/HttpUrl.h"

namespace haha{

/* ************************************************HTTP URL************************************************ */

HttpUrl::HttpUrl(std::string_view url, bool decode){
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
    size_t n = url_.size();
    std::string_view url_view(url_.c_str());
    size_t pos;

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
        host_ = url_view;
        hostName_ = host_;
        path_ = "/";
        return PARSE_SUCCESS;
    }
    else{
        host_ = url_view.substr(0, pos);
        size_t pos1 = host_.find(":");
        if(pos1 != host_.npos){
            hostName_ = host_.substr(0, pos1);
            port_ = host_.substr(pos1+1);
        }
    }

    // 解析path
    url_view.remove_prefix(pos); // '/'要保留
    pos = url_view.find('?');
    if(pos == url_view.npos){
        path_ = url_view;
        return PARSE_SUCCESS;
    }
    else{
        path_ = url_view.substr(0, pos);
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
            query_[key] = val;
            key = "";
            val = "";
        }
    }
    if(!key.empty()){
        val = query_strview.substr(j, i-j);
        query_[key] = val;
    }

    return PARSE_SUCCESS;
}

}