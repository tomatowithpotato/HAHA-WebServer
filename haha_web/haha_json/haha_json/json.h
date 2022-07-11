#ifndef __HAHA_JSON_JSON_H__
#define __HAHA_JSON_JSON_H__

#include "jsonValue.h"
#include "jsonTypeCast.h"
#include "jsonUtil.h"
#include "jsonBuffer.h"
#include <string>
#include <vector>
#include <list>

namespace haha
{

namespace json
{

JsonNode::ptr parse(const char *str);
JsonNode::ptr parse(const std::string &str);

JsonNode::ptr parse(std::string_view str);
JsonNode::ptr parse_value(std::string_view &str);
JsonNode::ptr parse_string(std::string_view &str);
JsonNode::ptr parse_number(std::string_view &str);
JsonNode::ptr parse_array(std::string_view &str);
JsonNode::ptr parse_object(std::string_view &str);

JsonNode::ptr fromString(const char* str);
JsonNode::ptr fromString(const std::string &str);

JsonNode::ptr fromFile(const char* filePath);
JsonNode::ptr fromFile(const std::string &filePath);

void toFile(JsonNode::ptr, const char* filePath, 
            const PrintFormatter &fmter = {});
void toFile(JsonNode::ptr, const std::string &filePath, 
            const PrintFormatter &fmter = {});

void toFile(JsonNode::ptr, const char* filePath, 
            JsonFormatType t, int indent);
void toFile(JsonNode::ptr, const std::string &filePath, 
            JsonFormatType t, int indent);

} // namespace json


} // namespace haha


#endif