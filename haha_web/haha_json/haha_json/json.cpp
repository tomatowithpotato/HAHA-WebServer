#include "json.h"
#include <limits.h>

namespace haha
{

namespace json
{

/* ---------------------------------------------parse--------------------------------------------- */

JsonNode::ptr parse_string(std::string_view &str){
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
                    str.remove_prefix(2);
                    break;
                case 't':
                    output += '\t';
                    str.remove_prefix(2);
                    break;
                case 'n':
                    output += '\n';
                    str.remove_prefix(2);
                    break;
                case 'r':
                    output += '\r';
                    str.remove_prefix(2);
                    break;
                case '0':
                    output += '\0';
                    str.remove_prefix(2);
                    break;
                case 'b':
                    output += '\b';
                    str.remove_prefix(2);
                    break;
                case 'f':
                    output += '\f';
                    str.remove_prefix(2);
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

    return std::static_pointer_cast<JsonNode>(jstr);
}


JsonNode::ptr parse_number(std::string_view &str){
    if(!isdigit(str[0]) && str[0] != '-')return nullptr;

    bool isInt = true;
    double number = 0;
    std::string number_str;
    while(!str.empty() && util::isNumberComponent(str[0])){
        isInt = str[0] != '.';
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

    JsonNode::ptr res = nullptr;
    if(isInt && number <= INT_MAX && number >= INT_MIN){
        res = std::make_shared<JsonInteger>((int)number);
    }
    else{
        res = std::make_shared<JsonDouble>(number);
    }

    return res;
}


JsonNode::ptr parse_array(std::string_view &str){
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

    return std::static_pointer_cast<JsonNode>(arr);
}


JsonNode::ptr parse_object(std::string_view &str){
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

        obj->add(std::static_pointer_cast<JsonString>(k), v);

        str = util::skip_CtrlAndSpace(str);

    }while(!str.empty() && check_func());

    if(str[0] != '}'){
        return nullptr;
    }
    str.remove_prefix(1);

    return std::static_pointer_cast<JsonNode>(obj);
}


JsonNode::ptr parse_value(std::string_view &str){
    if(str.empty())return nullptr;

    if(str.size() && (str[0] == '{')){
        return parse_object(str);
    }
    if(str.size() && (str[0] == '[')){
        return parse_array(str);
    }
    if(str.size() && (int)str[0] == '"'){
        return parse_string(str);
    }
    if(str.size() && (str[0] == '-' || isdigit(str[0]))){
        return parse_number(str);
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

JsonNode::ptr parse(std::string_view str){
    auto view = str;
    view = util::skip_utf8_bom(view);
    view = util::skip_CtrlAndSpace(view);
    auto obj = parse_value(view);
    return obj;
}

JsonNode::ptr parse(const char *str){
    std::string_view str_view = str;
    return parse(str_view);
}

JsonNode::ptr parse(const std::string &str){
    std::string_view str_view = str;
    return parse(str_view);
}

JsonNode::ptr fromString(const char* str){
    std::string_view view = str;
    return parse(view);
}

JsonNode::ptr fromString(const std::string &str){
    std::string_view view = str;
    return parse(view);
}


JsonNode::ptr fromFile(const char* filePath){
    JsonBuffer buffer;
    buffer.readFile(filePath);
    return parse(buffer.to_stringview());
}


JsonNode::ptr fromFile(const std::string &filePath){
    JsonBuffer buffer;
    buffer.readFile(filePath.c_str());
    return parse(buffer.to_stringview());
}


void toFile(JsonNode::ptr js, const char* filePath, const PrintFormatter &fmter){
    JsonBuffer buffer(js->toString(fmter));
    buffer.writeFile(filePath);
}


void toFile(JsonNode::ptr js, const std::string &filePath, const PrintFormatter &fmter){
    JsonBuffer buffer(js->toString(fmter));
    buffer.writeFile(filePath.c_str());
}

void toFile(JsonNode::ptr js, const char* filePath, 
            JsonFormatType t, int indent)
{
    toFile(js, filePath, {t,indent});

}
void toFile(JsonNode::ptr js, const std::string &filePath, 
            JsonFormatType t, int indent)
{
    toFile(js, filePath, {t,indent});
}

} // namespace json

}