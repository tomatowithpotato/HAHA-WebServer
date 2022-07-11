#include <iostream>
#include <string>
#include "json.h"


namespace JSON = haha::json;

int main(){
    std::string str = "{\"check\": 123.5e10, \"2893h\":\"ok\", \"arr\": [\"sd\", null]}";
    JSON::Json json;

    bool ok = true;
    ok = json.fromString(str);

    std::cout << json.toString() << std::endl;

    auto p = json.getValuePtr();

    auto obj = JSON::pointer_cast<JSON::JsonObject>(p);

    std::cout << obj->toString() << std::endl;

    return 0;
}