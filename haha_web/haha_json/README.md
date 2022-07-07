# HAHAWebServer

一个用c++17实现的json解析库，还在更新中

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

代码在tests文件夹下的test_parse.cc
```c++
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

    if(obj){
        for(const auto &[k,v] : *obj){
            std::cout << haha::json::getJsonTypeName(k.getType()) << ": "
                << haha::json::getJsonTypeName(v->getType())
                << std::endl;
        }

        haha::json::PrintFormatter fmt{
            haha::json::JsonFormatType::NEWLINE,
            1,
        };
        std::cout << obj->toString(fmt) << std::endl;
    }

    return 0;
}
```