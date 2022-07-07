#ifndef __HAHA_JSON_JSON_H__
#define __HAHA_JSON_JSON_H__

#include "jsonValue.h"
#include "jsonUtil.h"
#include "jsonReader.h"
#include <string>
#include <vector>
#include <list>

namespace haha
{

namespace json
{

JsonValue::ptr parse(const char *str);
JsonValue::ptr parse(const std::string &str);

JsonValue::ptr parse(std::string_view &str);
JsonValue::ptr parse_value(std::string_view &str);
JsonString::ptr parse_string(std::string_view &str);
JsonNumber::ptr parse_number(std::string_view &str);
JsonArray::ptr parse_array(std::string_view &str);
JsonObject::ptr parse_object(std::string_view &str);

class Json{
public:
    typedef std::shared_ptr<Json> ptr;

    bool fromString(const char *string);
    bool fromString(const std::string &str);
    bool fromString(std::string_view str);

    std::string toString() const { return obj_ ? obj_->toString() : "---failed parse---"; }

    bool fromFile(const char* filePath);
    bool fromFile(const std::string &filePath);

    bool toFile(const char* filePath);
    bool toFile(const std::string &filePath);

    JsonType getType() const { return obj_ ? obj_->getType() : JsonType::UNKOWN; }

    template<typename T>
    std::shared_ptr<T> getValuePtr() const { return std::static_pointer_cast<T>(obj_); }

private:
    std::string_view view_;
    JsonValue::ptr obj_;
    JsonReader reader_;
};

} // namespace json


} // namespace haha


#endif