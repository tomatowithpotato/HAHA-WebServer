#include "http/HttpUtil.h"
#include <cstring>

namespace haha{
/* ************************************************解析键值对************************************************ */

void parseKeyValue(const std::string& src, const std::string& join, const std::string& split,
                                    HttpMap<CASE_SENSITIVE::YES> &map, bool urldecode){
    size_t i;
    size_t j = 0;
    std::string_view view;
    std::string decoded_src;
    if(urldecode){
        decoded_src = EncodeUtil::urlDecode(src);
        view = decoded_src;
    }
    else{
        view = src;
    }

    while(!view.empty()){
        size_t pos = view.find(split);
        std::string_view kv = view.substr(0,pos);
        size_t pos1 = kv.find(join);
        if(pos1 == kv.npos){
            // 缺少分隔符，解析失败，停止解析后续内容
            break;
        }
        std::string_view k = kv.substr(0, pos1);
        std::string_view v = kv.substr(pos1+join.size());
        map.add(std::string(k), std::string(v));
        if(pos == view.npos){
            break;
        }
        else{
            pos += split.size();
            while(pos < view.size() && view[pos] == ' '){
                ++pos;
            }
        }
        view.remove_prefix(pos);
    }
}

}