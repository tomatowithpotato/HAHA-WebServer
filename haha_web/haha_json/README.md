# HAHA_JSON

一个用c++17实现的json解析库，还在更新中

## 技术特点

* 使用了c++17中的variant和string_view

* 多态、泛型编程

* 使用智能指针管理内存

* 支持读取ANSI、utf8、utf16编码，统一解析为utf8格式

## 未来的计划：

* 提供更易用的API

* 性能优化


## 快速使用：

编译
```shell
cd 当前目录
./build_release.sh
```
运行
```shell
cd 当前目录
./run_jsonTest_release.sh
```


## 代码示例

代码在tests文件夹下的test_json.cc
```c++
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

    // 输出到文件
    JSON::toFile(js1, "../output.json", fmt1);

    return 0;
}
```
