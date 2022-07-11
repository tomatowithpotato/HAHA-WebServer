#include "jsonValue.h"

using namespace haha::json;

namespace haha{

namespace json
{

/* 实际值类型V映射到json数据类型T */
template<typename V> struct Value2JClassMap;
template<> struct Value2JClassMap<std::string> { using T = JsonString; };
template<> struct Value2JClassMap<int> { using T = JsonInteger; };
template<> struct Value2JClassMap<double> { using T = JsonDouble; };
template<> struct Value2JClassMap<bool> { using T = JsonBoolean; };
template<> struct Value2JClassMap<decltype(nullptr)> { using T = JsonNull; };
template<> struct Value2JClassMap<JsonArray::ValueType> { using T = JsonArray; };
template<> struct Value2JClassMap<JsonObject::ValueType> { using T = JsonObject; };

/* json枚举类型jtype映射json数据类型T */
template<JsonType jtype> struct JType2JClassMap;
template<> struct JType2JClassMap<JsonType::String> { using T = JsonString; };
template<> struct JType2JClassMap<JsonType::Integer> { using T = JsonInteger; };
template<> struct JType2JClassMap<JsonType::Double> { using T = JsonDouble; };
template<> struct JType2JClassMap<JsonType::Boolean> { using T = JsonBoolean; };
template<> struct JType2JClassMap<JsonType::Null> { using T = JsonNull; };
template<> struct JType2JClassMap<JsonType::Array> { using T = JsonArray; };
template<> struct JType2JClassMap<JsonType::Object> { using T = JsonObject; };

/* json枚举类型jtype映射实际值类型T */
template<JsonType jtype> struct JType2ValueMap;
template<> struct JType2ValueMap<JsonType::String> { using T = std::string; };
template<> struct JType2ValueMap<JsonType::Integer> { using T = int; };
template<> struct JType2ValueMap<JsonType::Double> { using T = double; };
template<> struct JType2ValueMap<JsonType::Boolean> { using T = bool; };
template<> struct JType2ValueMap<JsonType::Null> { using T = decltype(nullptr); };
template<> struct JType2ValueMap<JsonType::Array> { using T = JsonArray::ValueType; };
template<> struct JType2ValueMap<JsonType::Object> { using T = JsonObject::ValueType; };


/* 判断json数据类型J是否匹配值类型V */
template<typename J, typename V> 
struct is_Jclass_Match_Value{
    bool operator() (){
        return std::is_same<J, typename J::ValueType>::value;
    }
};

} // namespace json


}


