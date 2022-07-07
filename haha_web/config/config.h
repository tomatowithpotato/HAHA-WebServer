#ifndef __HAHA_CONFIG_H__
#define __HAHA_CONFIG_H__

#include <string>
#include "haha_json/json.h"
#include "base/noncopyable.h"

namespace haha
{

static const char* defaultConfigFile = "./base_config.json";

class Config : public noncopyable {
public:
    Config& getInstance(){
        static Config config;
        return config;
    }
private:
    bool success_ = false;
    Config() {
        json::Json json;
        success_ = json.fromFile(defaultConfigFile);
    }
};

} // namespace haha

#endif