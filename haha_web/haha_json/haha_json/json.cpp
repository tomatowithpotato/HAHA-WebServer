#include "json.h"

namespace haha
{

namespace json
{

/* ---------------------------------------------parse--------------------------------------------- */

JsonString::ptr parse_string(std::string_view &str){
    if(str[0] != '"')return nullptr;
    str.remove_prefix(1);

    std::string output;
    while(!str.empty() && str[0] != '"'){
        if(str[0] == '\\'){
            if(str.size() < 2){
                return nullptr;
            }
            // 处理转义字符
            switch (str[1]) 
            {
                case '\\':
                    output += '\\';
                    break;
                case 't':
                    output += '\t';
                    break;
                case 'n':
                    output += '\n';
                    break;
                case 'r':
                    output += '\r';
                    break;
                case '0':
                    output += '\0';
                    break;
                case 'b':
                    output += '\b';
                    break;
                case 'f':
                    output += '\f';
                    break;
                // 十六进制值
                case 'x':
                    {
                        std::string hexs;
                        str.remove_prefix(2);
                        while(util::isHex(str[0])){
                            hexs += str[0];
                            str.remove_prefix(1);
                        }
                        if(hexs.empty()){
                            return nullptr;
                        }
                        output += std::move(std::to_string(util::toBase<int, 16>(hexs)));
                        break;
                    }
                // utf编码
                case 'u':
                    {
                        auto temp = std::move(util::utf16_literal_to_utf8(str));
                        if(temp.empty()){
                            return nullptr;
                        }
                        output += std::move(temp);
                        break;
                    }
                default:
                    return nullptr;
            }
        }
        else{
            output += str[0];
            str.remove_prefix(1);
        }
    }

    if(str.empty() || str[0] != '"')return nullptr;
    str.remove_prefix(1);

    JsonString::ptr jstr(std::make_shared<JsonString>(output));
    return jstr;
}


JsonNumber::ptr parse_number(std::string_view &str){
    if(!isdigit(str[0]) && str[0] != '-')return nullptr;

    double number = 0;
    std::string number_str;
    while(!str.empty() && util::isNumberComponent(str[0])){
        number_str += str[0];
        str.remove_prefix(1);
    }
    try{
        number = std::stod(number_str);
    }
    catch(std::exception& e)
    {
        return nullptr;
    }

    return std::make_shared<JsonNumber>(number);
}


JsonArray::ptr parse_array(std::string_view &str){
    if(str[0] != '[')return nullptr;
    str.remove_prefix(1);

    JsonArray::ptr arr(std::make_shared<JsonArray>());

    if(!str.empty() && str[0] == ']'){
        str.remove_prefix(1);
        return arr;
    }

    auto check_func = [&str](){
        if(str[0] == ','){
            str.remove_prefix(1);
            /* 支持最后一个加逗号 */
            if(str.size() && str[0] == ']'){
                return false;
            }
            return true;
        }
        return false;
    };

    do{
        str = util::skip_CtrlAndSpace(str);
        if(str.empty()){return nullptr;}
        auto jv = parse_value(str);
        if(jv == nullptr){
            return nullptr;
        }
        arr->add(jv);
        str = util::skip_CtrlAndSpace(str);
    }while(!str.empty() && check_func());

    if(str[0] != ']'){
        return nullptr;
    }
    str.remove_prefix(1);

    return arr;
}


JsonObject::ptr parse_object(std::string_view &str){
    if(str[0] != '{')return nullptr;
    str.remove_prefix(1);

    JsonObject::ptr obj(std::make_shared<JsonObject>());

    if(!str.empty() && str[0] == '}'){
        str.remove_prefix(1);
        return obj;
    }

    auto check_func = [&str](){
        if(str[0] == ','){
            str.remove_prefix(1);
            /* 支持最后一个加逗号 */
            if(str.size() && str[0] == '}'){
                return false;
            }
            return true;
        }
        return false;
    };

    do{
        str = util::skip_CtrlAndSpace(str);
        if(str.empty()){return nullptr;}

        // 解析键
        auto k = parse_string(str);
        if(k == nullptr){
            return nullptr;
        }

        str = util::skip_CtrlAndSpace(str);

        // 分隔符
        if(str[0] != ':'){
            return nullptr;
        }
        str.remove_prefix(1);

        str = util::skip_CtrlAndSpace(str);

        // 解析值
        if(str.empty()){return nullptr;}
        auto v = parse_value(str);
        if(v == nullptr){
            return nullptr;
        }

        obj->add(*std::static_pointer_cast<JsonString>(k), v);

        str = util::skip_CtrlAndSpace(str);

    }while(!str.empty() && check_func());

    if(str[0] != '}'){
        return nullptr;
    }
    str.remove_prefix(1);

    return obj;
}


JsonValue::ptr parse_value(std::string_view &str){
    if(str.empty())return nullptr;

    if(str.size() && (str[0] == '{')){
        return std::static_pointer_cast<JsonValue>(parse_object(str));
    }
    if(str.size() && (str[0] == '[')){
        return std::static_pointer_cast<JsonValue>(parse_array(str));
    }
    if(str.size() && (int)str[0] == '"'){
        return std::static_pointer_cast<JsonValue>(parse_string(str));
    }
    if(str.size() && (str[0] == '-' || isdigit(str[0]))){
        return std::static_pointer_cast<JsonValue>(parse_number(str));
    }
    if(str.size() >= 4 && str.compare(0, 4, "null") == 0){
        str.remove_prefix(4);
        return std::make_shared<JsonNull>();
    }
    if(str.size() >= 4 && str.compare(0, 4, "true") == 0){
        str.remove_prefix(4);
        return std::make_shared<JsonBoolean>(true);
    }
    if(str.size() >= 5 && str.compare(0, 5, "false") == 0){
        str.remove_prefix(5);
        return std::make_shared<JsonBoolean>(false);
    }
    else{
        return nullptr;
    }
}

JsonValue::ptr parse(std::string_view &str){
    str = util::skip_utf8_bom(str);
    str = util::skip_CtrlAndSpace(str);
    auto obj = parse_value(str);
    return obj;
}

JsonValue::ptr parse(const char *str){
    std::string_view str_view = str;
    return parse(str_view);
}

JsonValue::ptr parse(const std::string &str){
    std::string_view str_view = str;
    return parse(str_view);
}


/* ---------------------------------------------Json--------------------------------------------- */

bool Json::fromString(std::string_view str){
    obj_ = json::parse(str);
    return obj_ != nullptr;
}

bool Json::fromString(const char *str){
    std::string_view str_view = str;
    return fromString(str_view);
}

bool Json::fromString(const std::string &str){
    std::string_view str_view = str;
    return fromString(str_view);
}


bool Json::fromFile(const char *string){
    reader_.readFile(string);
    return fromString(reader_.to_stringview());
}


bool Json::fromFile(const std::string &string){
    reader_.readFile(string.c_str());
    return fromString(reader_.to_stringview());
}

} // namespace json

}