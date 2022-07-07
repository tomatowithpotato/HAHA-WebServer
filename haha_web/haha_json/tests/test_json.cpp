#include <string>
#include <iostream>
#include "json.h"


int main(){
    std::string str = "{\"check\": 123.5e10, \"2893h\":\"ok\", \"arr\": [\"sd\", null]}";
    std::string str1 = "{\"\": 0, \"\\u7814\": 1, \"\\u7a76\": 2, \"\\u53d1\": 3, \"\\u73b0\": 4, \"\\u7ec6\": 5, \"\\u80de\": 6}";
    haha::json::Json json;

    bool ok = true;
    ok = json.fromString(str1);

    std::cout << ok << std::endl;

    std::cout << json.toString() << std::endl;

    std::string filePath = "../test.json"; // 文件位置自己定

    ok = json.fromFile(filePath);

    std::cout << ok << std::endl;

    std::cout << json.toString() << std::endl;

    haha::json::JsonObject::ptr obj;
    if(json.getType() == haha::json::JsonType::Object){
        obj = json.getValuePtr<haha::json::JsonObject>();
    }

    for(const auto &[k,v] : *obj){
        std::cout << haha::json::getJsonTypeName(k.getType()) << ": "
            << haha::json::getJsonTypeName(v->getType())
            << std::endl;
    }

    haha::json::PrintFormatter fmt{
        haha::json::JsonFormatType::NEWLINE,
        1,
        true
    };

    /* 序列化 */
    std::string output = obj->toString(fmt);
    std::cout << output << std::endl;

    std::cout << std::string(60, '*') << std::endl;

    /* 反序列化 */
    haha::json::Json json1;
    ok = json1.fromString(output);
    std::cout << ok << std::endl;
    haha::json::JsonObject::ptr obj1;
    if(json1.getType() == haha::json::JsonType::Object){
        obj1 = json.getValuePtr<haha::json::JsonObject>();
    }

    haha::json::PrintFormatter fmt1{
        haha::json::JsonFormatType::NEWLINE,
        1,
        false
    };
    
    if(ok){
        std::string output1 = obj1->toString(fmt1);
        std::cout << output1 << std::endl;
    }

    return 0;
}