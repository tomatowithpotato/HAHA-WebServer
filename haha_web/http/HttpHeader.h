#ifndef __HAHA_HTTPHEADER_H__
#define __HAHA_HTTPHEADER_H__

#include <vector>
#include <string>
#include "http/HttpMap.h"

namespace haha{

// class HttpField{
// public:
//     HttpField(const std::string& key, const std::string& val,
//                 const std::string &split, const std::string &stop)
//         :key_(key){
        
//     }
// private:
//     std::string key_;
//     std::vector<std::string> vals_;
//     std::string split_;
//     std::string stop_;
// };

class HttpHeader : public HttpMap<CASE_SENSITIVE::NO>{
public:
    HttpHeader():length_(0){}

    void add(const std::string& key, const std::string& value) override{
        map_[toLowers(key)] = value;
        length_ += key.size() + value.size();
    }

    void del(const std::string& key) override {
        ConstIterator it;
        it = map_.find(toLowers(key));
        
        if(it != map_.end()){
            length_ -= key.size() + it->second.size();
            map_.erase(it);
        }
    }

    // 返回键值对数量
    size_t keyCount() const { return map_.size(); }

    // 返回总大小
    size_t size() const override { return length_; }

    bool empty() const override { return length_ > 0; }

    RetOption<std::string> getCookie() const { return get("Cookie"); }

    RetOption<std::string> getContentLength() const { return get("Content-Length"); }

    RetOption<std::string> getTransferEncoding() const { return get("Transfer-Encoding"); } 

    RetOption<std::string> getContentType() const { return get("Content-Type"); }

    RetOption<std::string> getConnection() const { return get("Connection"); }

    RetOption<std::string> getAcceptEncoding() const { return get("Accept-Encoding"); }

    void setCookie(const std::string &val) { add("Cookie", val); }

    void setContentLength(const std::string &val) { add("Content-Length", val); }

    void setTransferEncoding(const std::string &val) { add("Transfer-Encoding", val); } 

    void setContentType(const std::string &val) { add("Content-Type", val); }

    void setConnection(const std::string &val) { add("Connection", val); }

    void setAcceptEncoding(const std::string &val) { add("Accept-Encoding", val); }

private:
    size_t length_;

};

}


#endif