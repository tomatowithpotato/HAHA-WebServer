#ifndef __HAHA_HTTPHEADER_H__
#define __HAHA_HTTPHEADER_H__

#include <vector>
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
    RetOption<std::string> Cookie() const { return get("Cookie"); }

    RetOption<std::string> Content_Length() const { return get("Content-Length"); }

    RetOption<std::string> Transfer_Encoding() const { return get("Transfer-Encoding"); } 

    RetOption<std::string> Content_Type() const { return get("Content-Type"); }

    RetOption<std::string> Connection() const { return get("Connection"); }

    RetOption<std::string> Accept_Encoding() const { return get("Accept-Encoding"); }
};

}


#endif