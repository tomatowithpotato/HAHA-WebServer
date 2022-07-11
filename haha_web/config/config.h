#ifndef __HAHA_CONFIG_H__
#define __HAHA_CONFIG_H__

#include <string>
#include <variant>
#include <exception>
#include <unordered_map>
#include "log/Log.h"
#include "haha_json/json.h"
#include "base/noncopyable.h"
#include "base/RetOption.h"

#define GET_CONFIG(T, str, default_val) Config::getInstance().query<T>(str,default_val)

namespace haha
{

static const char* defaultConfigFile = "../configs/base_config.json";


class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    explicit ConfigVarBase(const std::string &desc = ""):description_(desc){}
    std::string getDescription() const { return description_; }
private:
    std::string description_;
};

template<class T>
class ConfigVar : public ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    ConfigVar(const T& var, const std::string &desc = ""):ConfigVarBase(desc),val_(var){}
    const T& getValue() const { return val_; }
protected:
    T val_;
};


class Config : public noncopyable {
public:
    using Values = std::variant<std::string, int, bool, std::vector<std::string>>;

    static Config& getInstance(){
        static Config config;
        return config;
    }

    /* 
    输入格式: config.server.port 
    */
    template<typename T> const T& query(const char* query_str, const T& default_value);
    
private:
    Config() {
        json_ = json::fromFile(defaultConfigFile);
        if(json_ == nullptr){
            HAHA_LOG_ERROR(HAHA_LOG_ROOT()) << "read file error";
            return;
        }
        if(json_->getType() != json::JsonType::Object){
            HAHA_LOG_ERROR(HAHA_LOG_ROOT()) << "json should be object";
            return;
        }
        std::list<std::pair<std::string, json::JsonNode::ptr> > all_nodes;
        ListAllMember("", json_, all_nodes);

        for(const auto &it : all_nodes){
            map_.insert({it.first, it.second});
        }

        HAHA_LOG_INFO(HAHA_LOG_ROOT()) << map_.size() << " configuration items detected";
    }

    // 生成形如a.b.c的键的键值对
    void ListAllMember(const std::string& prefix, 
                        json::JsonNode::ptr node, 
                        std::list<std::pair<std::string, json::JsonNode::ptr> >& output)
    {
        if(prefix.size()){
            output.push_back({prefix, node});
        }
        if(node->isObject()){
            json::JsonObject::ptr obj = json::pointer_cast<json::JsonObject>(node);
            for(const auto &it : *obj){
                ListAllMember(prefix.empty() ? it.first.getValue()
                    : prefix + "." + it.first.getValue(), it.second, output);
            }
        }            
    }

private:
    bool success_ = false;
    json::JsonNode::ptr json_;
    std::unordered_map<std::string, json::JsonNode::ptr> map_;
};

} // namespace haha

#endif