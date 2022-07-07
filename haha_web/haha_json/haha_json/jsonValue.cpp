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


std::string JsonString::toString(const PrintFormatter &format, int depth) const{
    std::string res;
    res.reserve(val_.size());
    res += '"';

    std::string_view view(val_);
    char utf_str[6] = {0};
    while(!view.empty()){
        // 普通字符
        if(view[0] > 31 && view[0] != '\"' && view[0] != '\\'){
            res += view[0];
        }
        else{
            res += '\\';
            switch (view[0])
            {
            case '\\':
                res += '\\';
                break;
            case '\"':
                res += '"';
                break;
            case '\b':
                res += 'b';
                break;
            case '\f':
                res += 'f';
                break;
            case '\n':
                res += 'n';
                break;
            case '\r':
                res += 'r';
                break;
            case '\t':
                res += 't';
                break;
            default:
                // uxxxx
                if(format.ensureAscii()){
                    sprintf(utf_str, "u%04x", *(unsigned char *)view.data());
                    res += utf_str;
                }
                else{
                    res.pop_back();
                    res += view[0];
                }
                break;
            }
        }
        view.remove_prefix(1);
    }
    res += '"';
    return res;
}


} // namespace json


} // namespace haha