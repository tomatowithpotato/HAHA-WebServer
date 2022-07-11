#include "config/config.h"

namespace haha
{

template<>
const std::string& Config::query<std::string>(const char* query_str, const std::string& default_value){
    auto it = map_.find(query_str);
    if(it == map_.end()){
        return default_value;
    }
    auto node = it->second;
    auto t = node->getType();
    if(t == JsonType::String){
        const auto p = json::pointer_cast<JsonString>(node);
        return p->getValue();
    }
    return default_value;
}

template<>
const int& Config::query<int>(const char* query_str, const int &default_value){
    auto it = map_.find(query_str);
    if(it == map_.end()){
        return default_value;
    }
    auto node = it->second;
    auto t = node->getType();
    if(t == JsonType::Integer){
        const auto p = json::pointer_cast<JsonInteger>(node);
        return p->getValue();
    }
    return default_value;
}

template<>
const double& Config::query<double>(const char* query_str, const double& default_value){
    auto it = map_.find(query_str);
    if(it == map_.end()){
        return default_value;
    }
    auto node = it->second;
    auto t = node->getType();
    if(t == JsonType::Double){
        const auto p = json::pointer_cast<JsonDouble>(node);
        return p->getValue();
    }
    return default_value;
}

template<>
const bool& Config::query<bool>(const char* query_str, const bool& default_value){
    auto it = map_.find(query_str);
    if(it == map_.end()){
        return default_value;
    }
    auto node = it->second;
    auto t = node->getType();
    if(t == JsonType::Boolean){
        const auto p = json::pointer_cast<JsonBoolean>(node);
        return p->getValue();
    }
    return default_value;
}

} // namespace haha
