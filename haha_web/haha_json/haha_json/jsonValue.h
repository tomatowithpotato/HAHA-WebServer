#ifndef __HAHA_JSON_JSONVALUE_H__
#define __HAHA_JSON_JSONVALUE_H__

#include <memory>
#include <vector>
#include <variant>
#include <map>
#include "jsonUtil.h"
#include "jsonError.h"


namespace haha
{

namespace json
{

enum class JsonType { UNKOWN, Object, Array, String, Integer, Double, Boolean, Null };

typedef decltype(nullptr) NullType;

std::string getJsonTypeName(JsonType json_type);

enum class JsonFormatType { RAW, SPACE, NEWLINE };

class PrintFormatter{
public:
    PrintFormatter(const JsonFormatType &fmt = JsonFormatType::RAW, 
                    int indent = 0)
                    // bool ensure_ascii = true)
        :format_(fmt),
        indent_(indent)
        // ensure_ascii_(ensure_ascii)
    {
    }
    JsonFormatType formatType() const { return format_; }
    // bool ensureAscii() const { return ensure_ascii_; }
private:
    JsonFormatType format_ = JsonFormatType::RAW;
    int indent_ = 0;
    // bool ensure_ascii_ = true;
};

class JsonString;

class JsonNode{
public:
    typedef std::shared_ptr<JsonNode> ptr;
    explicit JsonNode(JsonType type = JsonType::UNKOWN):type_(type){}
    virtual ~JsonNode() {}

    JsonNode(const JsonNode& jsvb);

    JsonType getType() const { return type_; }
    // 是否为标量：字符串、数字、bool、null
    bool isScalar() const { return !isIterable(); }
    // 可迭代类型：对象、数组
    bool isIterable() const { return type_ == JsonType::Object || type_ == JsonType::Array; }
    
    bool isString() { return type_ == JsonType::String; }
    bool isNumber() { return isInteger() || isDouble(); }
    bool isInteger() { return type_ == JsonType::Integer; }
    bool isDouble() { return type_ == JsonType::Double; }
    bool isBoolean() { return type_ == JsonType::Boolean; }
    bool isNull() { return type_ == JsonType::Null; }
    bool isArray() { return type_ == JsonType::Array; }
    bool isObject() { return type_ == JsonType::Object; }

    virtual std::string toString (const PrintFormatter &format = PrintFormatter(), int depth = 0) const { return ""; }

    // std::string toString (bool ensure_ascii) const {
    //     return toString({JsonFormatType::RAW, 0, ensure_ascii});
    // }
    // std::string toString (int indent, bool ensure_ascii) const {
    //     return toString({JsonFormatType::RAW, indent, ensure_ascii});
    // }
    // std::string toString (const JsonFormatType &fmt, int indent, bool ensure_ascii) const {
    //     return toString({fmt, indent, ensure_ascii});
    // }
    std::string toString (int indent) const {
        return toString({JsonFormatType::RAW, indent});
    }
    std::string toString (const JsonFormatType &fmt, int indent) const {
        return toString({fmt, indent});
    }

    virtual JsonNode& operator[](const std::string &key){
        throw HAHA_JSON_ERROR("do not support operator[]");
    }
    virtual JsonNode& operator[](unsigned key){
        throw HAHA_JSON_ERROR("do not support operator[]");
    }

protected:
    JsonNode::ptr copyFrom(JsonNode::ptr src);

protected:
    JsonType type_;
    std::variant<bool, int, double,
                decltype(nullptr),
                std::string,
                std::vector<JsonNode::ptr>,
                std::map<JsonString, JsonNode::ptr>> val_ = nullptr;
};


template<typename T>
class JsonValue : public JsonNode{
public:
    using ValueType = T;
    JsonValue(JsonType type, const T& val)
        :JsonNode(type){ val_ = val; }
    JsonValue():JsonNode(){}
    const T& getValue() const { return std::get<ValueType>(val_); }
    T& getValue() { return std::get<ValueType>(val_); }
};


class JsonString : public JsonValue<std::string>{
public:
    typedef std::shared_ptr<JsonString> ptr;
    explicit JsonString(const std::string &str):JsonValue(JsonType::String, str){}
    // JsonString(const JsonString& another);
    std::string toString(const PrintFormatter &format = PrintFormatter(), int depth = 0) const override;
    bool operator <(const JsonString &rhs) const{
        return getValue() < rhs.getValue();
    }
};


class JsonBoolean : public JsonValue<bool>{
public:
    typedef std::shared_ptr<JsonBoolean> ptr;
    explicit JsonBoolean(bool val):JsonValue(JsonType::Boolean, val){}
    std::string toString(const PrintFormatter &format = PrintFormatter(), int depth = 0) const override { 
        return getValue() ? "true" : "false";
    }
};


template<typename T, JsonType JTYPE>
class JsonNumber : public JsonValue<T>{
public:
    typedef std::shared_ptr<JsonNumber> ptr;
    explicit JsonNumber(const T& val):JsonValue<T>(JTYPE, val){}
    std::string toString(const PrintFormatter &format = PrintFormatter(), int depth = 0) const override { 
        return std::to_string(JsonValue<T>::getValue()); 
    }
};

using JsonInteger = JsonNumber<int, JsonType::Integer>;

using JsonDouble = JsonNumber<double, JsonType::Double>;


class JsonNull : public JsonValue<decltype(nullptr)>{
public:
    typedef std::shared_ptr<JsonNull> ptr;
    JsonNull():JsonValue(JsonType::Null, nullptr){}
    std::string toString(const PrintFormatter &format = PrintFormatter(), int depth = 0) const override { 
        return "null"; 
    }
};


class JsonArray : public JsonValue<std::vector<JsonNode::ptr>>{
public:
    typedef typename std::vector<JsonNode::ptr> Array;
    typedef typename Array::iterator Iterator;
    typedef typename Array::const_iterator ConstIterator;

    typedef std::shared_ptr<JsonArray> ptr;

    JsonArray() : JsonValue(JsonType::Array, Array()){}
    JsonArray(const JsonArray &another);

    ConstIterator begin() const { return getValue().begin(); }
    ConstIterator end() const { return getValue().end(); }
    Iterator begin() { return getValue().begin(); }
    Iterator end() { return getValue().end(); }

    size_t size() const { return getValue().size(); }
    bool empty() const { return getValue().empty(); }

    void add(JsonNode::ptr val) { getValue().emplace_back(val); }
    void add(const std::string &str) { getValue().emplace_back(std::make_shared<JsonString>(str)); }
    void add(bool val) { getValue().emplace_back(std::make_shared<JsonBoolean>(val)); }
    void add(int val) { getValue().emplace_back(std::make_shared<JsonInteger>(val)); }
    void add(double val) { getValue().emplace_back(std::make_shared<JsonDouble>(val)); }
    void add() { getValue().emplace_back(std::make_shared<JsonNull>()); }

    std::string toString(const PrintFormatter &format = PrintFormatter(), int depth = 0) const override {
        std::string res = "[";
        auto fmt = format.formatType();

        for(size_t i = 0; i < getValue().size(); ++i){
            if(fmt == JsonFormatType::NEWLINE){
                res += '\n';
                res += std::string(depth+1, '\t');
            }
            res += getValue()[i]->toString(format, depth+1);
            res += i+1 < getValue().size() ? "," : "";
            if(fmt == JsonFormatType::SPACE && i+1 < getValue().size()){
                res += ' ';
            }
        }

        res += fmt == JsonFormatType::NEWLINE && getValue().size() ? '\n' + std::string(depth, '\t') : "";
        res += "]";
        return res;
    }

    JsonArray& operator=(const JsonArray& another);

    JsonNode& operator[](unsigned key){
        return *getValue()[key];
    }
};


class JsonObject : public JsonValue<std::map<JsonString, JsonNode::ptr>>{
public:
    typedef typename std::map<JsonString, JsonNode::ptr> Map;
    typedef typename Map::iterator Iterator;
    typedef typename Map::const_iterator ConstIterator;

    typedef std::shared_ptr<JsonObject> ptr;
    typedef std::pair<std::string, JsonNode::ptr> kv_pair;

    JsonObject() : JsonValue(JsonType::Object, Map()){}

    JsonObject(const JsonObject& another);

    size_t size() const { return getValue().size(); }

    bool empty() const { return getValue().empty(); }

    ConstIterator begin() const { return getValue().begin(); }
    ConstIterator end() const { return getValue().end(); }
    Iterator begin() { return getValue().begin(); }
    Iterator end() { return getValue().end(); }

    void add(const JsonString::ptr key, JsonNode::ptr val){
        getValue().insert({*key, val});
    }
    void add(const JsonString key, JsonNode::ptr val){
        getValue().insert({key, val});
    }
    void add(const std::string &key, JsonNode::ptr val){
        getValue().insert({JsonString(key), val});
    }
    void add(const std::string &key, const std::string &val) { 
        getValue().insert({JsonString(key), std::make_shared<JsonString>(val)}); 
    }
    void add(const std::string &key, bool val) { 
        getValue().insert({JsonString(key), std::make_shared<JsonBoolean>(val)}); 
    }
    void add(const std::string &key, int val) { 
        getValue().insert({JsonString(key), std::make_shared<JsonInteger>(val)}); 
    }
    void add(const std::string &key, double val) { 
        getValue().insert({JsonString(key), std::make_shared<JsonDouble>(val)}); 
    }
    void add(const std::string &key) { 
        getValue().insert({JsonString(key), std::make_shared<JsonNull>()}); 
    }
    size_t del(const std::string &key) {
        return getValue().erase(JsonString(key));
    }

    template<typename T = JsonNode>
    T& get(const std::string &key){
        return *std::static_pointer_cast<T>(getValue().at(JsonString(key)));
    }

    std::string toString(const PrintFormatter &format = PrintFormatter(), int depth = 0) const override {
        std::string res = "{";
        auto fmt = format.formatType();
        int cnt = 0;
        for(const auto &[k, v] : getValue()){
            if(fmt == JsonFormatType::NEWLINE){
                res += '\n';
                res += std::string(depth+1, '\t');
            }
            res += k.toString(format, depth+1);
            res += ':';
            res += fmt == JsonFormatType::RAW ? "" : " ";
            res += v->toString(format, depth+1);
            res += (cnt + 1 < (int)getValue().size()) ? "," : "";
            res += (cnt + 1 < (int)getValue().size() && fmt == JsonFormatType::SPACE) ? " " : "";
            ++cnt;
        }
        res += fmt == JsonFormatType::NEWLINE && getValue().size() ? '\n' + std::string(depth, '\t') : "";
        res += '}';
        return res;
    }

    JsonObject& operator=(const JsonObject &another);

    JsonNode& operator[](const std::string& key){
        return get(key);
    }
};


/* 指针转换 */
template<typename T>
T::ptr pointer_cast(JsonNode::ptr source){
    static_assert(
        std::is_same<T, JsonString>::value ||
        std::is_same<T, JsonInteger>::value ||
        std::is_same<T, JsonDouble>::value ||
        std::is_same<T, JsonBoolean>::value ||
        std::is_same<T, JsonNull>::value ||
        std::is_same<T, JsonArray>::value ||
        std::is_same<T, JsonObject>::value,
        "error input type"
    );
    return std::static_pointer_cast<T>(source);
}

} // namespace json

} // namespace haha


#endif