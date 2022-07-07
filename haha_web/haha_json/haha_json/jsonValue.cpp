#include "jsonValue.h"

using namespace haha::json;

namespace haha
{

namespace json
{

/* ---------------------------------------------type conversion--------------------------------------------- */

template<>
std::shared_ptr<JsonString> To(JsonValue::ptr source){ 
    return std::static_pointer_cast<JsonString>(source);
}

template<>
std::shared_ptr<JsonBoolean> To(JsonValue::ptr source){ 
    return std::static_pointer_cast<JsonBoolean>(source);
}

template<>
std::shared_ptr<JsonNumber> To(JsonValue::ptr source){ 
    return std::static_pointer_cast<JsonNumber>(source);
}

template<>
std::shared_ptr<JsonArray> To(JsonValue::ptr source){ 
    return std::static_pointer_cast<JsonArray>(source);
}

template<>
std::shared_ptr<JsonObject> To(JsonValue::ptr source){ 
    return std::static_pointer_cast<JsonObject>(source);
}


std::string getJsonTypeName(JsonType json_type){
    switch (json_type)
    {
    case JsonType::Object:
        return "Object";
        break;
    case JsonType::Array:
        return "Array";
        break;
    case JsonType::String:
        return "String";
        break;
    case JsonType::Number:
        return "Number";
        break;
    case JsonType::Boolean:
        return "Boolean";
        break;
    case JsonType::Null:
        return "Null";
        break;
    default:
        break;
    }
    return "---error---";
}

} // namespace json


} // namespace haha