#include <memory>
#include <vector>
#include <variant>
#include <map>
#include <iostream>
#include "json.h"

namespace JSON = haha::json;

int main(){
    std::string str = "{\"check\": 123.5e10, \"2893h\":\"ok\", \"arr\": [\"sd\", null]}";

    JSON::Json json;

    bool ok = true;
    ok = json.fromString(str);

    auto obj1 = json.getValuePtr();

    std::cout << obj1->toString() << std::endl;

    auto fuck = JSON::pointer_cast<JSON::JsonObject>(obj1);
    JSON::JsonObject::ptr obj2(new JSON::JsonObject(*fuck));

    std::cout << JSON::getJsonTypeName(obj2->getType()) << std::endl;
    std::cout << obj2->toString() << std::endl;

    JSON::JsonObject obj3;
    obj3 = *obj2;
    std::cout << obj3.toString() << std::endl;

    obj2->add("nothing");
    auto cnt = obj2->del("arr");
    std::cout << cnt << std::endl;
    std::cout << obj2->toString() << std::endl;
    std::cout << obj3.toString() << std::endl;

    json.fromString("\"123\"");
    auto p = json.getValuePtr<JSON::JsonString>();

    JSON::JsonString hehe(*p);

    std::cout << hehe.toString() << std::endl;

    return 0;
}