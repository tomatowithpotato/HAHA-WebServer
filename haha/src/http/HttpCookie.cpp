#include "http/HttpCookie.h"

namespace haha{

HttpCookie::HttpCookie(const std::string &str)
    :secure_(false)
    ,httpOnly_(false){
    std::string decoded_str = EncodeUtil::urlDecode(str);
    std::string key;
    std::string val;
    size_t j = 0;
    size_t i = 0;
    while(i < decoded_str.size()){
        if(i == decoded_str.size()-1 || decoded_str[i] == ';'){
            i = decoded_str[i] == ';' ? i : i+1;
            if(key.empty()){
                std::string attr = toLowers(decoded_str.substr(j, i-j));
                if(attr == "secure"){
                    secure_ = true;
                }
                else if(attr == "httponly"){
                    httpOnly_ = true;
                }
            }
            else{
                std::string attr = toLowers(key);
                val = decoded_str.substr(j, i-j);
                if(attr == "domain"){
                    domain_ = val;
                }
                else if(attr == "path"){
                    path_ = val;
                }
                else if(attr == "expires"){
                    expires_ = val;
                }
                else if(attr == "max-age"){
                    maxAge_ = std::stoi(val);
                }
                else{
                    add(key, val);
                }
            }
            ++i;
            while(i < decoded_str.size() && decoded_str[i] == ' ')++i;
            j = i;
            key.clear();
            val.clear();
        }
        else if(decoded_str[i] == '='){
            key = decoded_str.substr(j, i-j);
            j = i + 1;
            ++i;
        }
        else{
            ++i;
        }
    }
}

std::string HttpCookie::toString() const{
    std::string str;
    int i = 0;
    int n = keyCount();
    for (const auto &[key, value] : map_) {
        str += key + "=" + value;
        if (i++ < n-1){
            str += "; ";
        }
    }
    if (!expires_.empty())
        str += "; Expires=" + expires_;
    if (!domain_.empty())
        str += "; Domain=" + domain_;
    if (!path_.empty())
        str += "; Path=" + path_;
    if (maxAge_ > 0)
        str += "; Max-Age=" + std::to_string(maxAge_);
    if (secure_)
        str += "; Secure";
    if (httpOnly_)
        str += "; HttpOnly";
    return str;
}


}