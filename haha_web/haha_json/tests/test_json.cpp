#include <string>
#include <iostream>
#include "json.h"

namespace JSON = haha::json;

int main(){
    JSON::JsonNode::ptr js;

    std::string filePath = "../test.json"; // 文件位置自己定

    // 从文件读取
    js = JSON::fromFile(filePath);

    // 序列化（转为字符串）
    std::cout << js->toString() << std::endl;

    // 类型转换
    JSON::JsonObject::ptr obj;
    if(js->getType() == JSON::JsonType::Object){
        obj = JSON::pointer_cast<JsonObject>(js);
    }

    // 输出格式
    JSON::PrintFormatter fmt{
        JSON::JsonFormatType::NEWLINE,
        1,
    };

    /* 序列化（转为字符串） */
    std::string output = obj->toString(fmt);
    std::cout << output << std::endl;

    std::cout << std::string(60, '*') << std::endl;

    /* 反序列化（从字符串读取） */
    JSON::JsonNode::ptr js1;
    js1 = JSON::fromString(output);

    JSON::PrintFormatter fmt1{
        JSON::JsonFormatType::NEWLINE,
        1,
    };
    
    std::string output1 = js1->toString(fmt1);
    std::cout << output1 << std::endl;

    std::cout << std::string(60, '*') << std::endl;

    // 直接通过基类[]方法访问
    JsonNode& conf = (*js1)["configurations"];
    std::cout << conf.toString() << std::endl;

    std::cout << std::string(60, '*') << std::endl;

    JsonNode& name = conf[0]["name"];
    std::cout << name.toString() << std::endl;

    // 输出到文件
    JSON::toFile(js1, "../output.json", fmt1);

    // 异常测试
    conf["yyk"];

    return 0;
}