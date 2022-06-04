#ifndef __HAHA_HTTPUTIL_H__
#define __HAHA_HTTPUTIL_H__

#include <unordered_map>
#include <string>
#include "base/util.h"
#include "base/RetOption.h"
#include "HttpMap.h"
#include "base/EncodeUtil.h"

#include <random>
#include <time.h>

namespace haha{

inline constexpr uint64_t hashExt(const char *s, size_t i, size_t l,
                                  uint64_t h) noexcept {
  return i == l ? h : hashExt(s, i + 1, l, (h * 31) + *(s + i));
}
inline uint64_t hashExt(const std::string_view &s) noexcept {
  return hashExt(s.data(), 0, s.size(), 0);
}
inline constexpr uint64_t operator""_h(const char *s, size_t n) noexcept {
  return hashExt(s, 0, n, 0);
}

enum HttpStatus {
  CONTINUE = 100,
  OK = 200,
  PARTIAL_CONTENT = 206,
  MOVED_PERMANENTLY = 301,
  FOUND = 302,
  NOT_MODIFIED = 304,
  TEMPORARY_REDIRECT = 307,
  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  METHOD_NOT_ALLOWED = 405,
  RANGE_NOT_SATISFIABLE = 416,
  INTERNAL_SERVER_ERROR = 500,
  SERVICE_UNAVAILABLE = 503,
  HTTP_VERSION_NOT_SUPPORTED = 505
};

inline const std::unordered_map<short, const char *> Status_Code = {
    {HttpStatus::CONTINUE, "Continue"},
    {HttpStatus::OK, "OK"},
    {HttpStatus::PARTIAL_CONTENT, "Partial Content"},
    {HttpStatus::MOVED_PERMANENTLY, "Moved Permanently"},
    {HttpStatus::FOUND, "Found"},
    {HttpStatus::NOT_MODIFIED, "Not Modified"},
    {HttpStatus::TEMPORARY_REDIRECT, "Temporary Redirect"},
    {HttpStatus::BAD_REQUEST, "Bad Request"},
    {HttpStatus::UNAUTHORIZED, "Unauthorized"},
    {HttpStatus::FORBIDDEN, "Forbidden"},
    {HttpStatus::NOT_FOUND, "Not Found"},
    {HttpStatus::METHOD_NOT_ALLOWED, "Method Not Allowed"},
    {HttpStatus::RANGE_NOT_SATISFIABLE, "Range Not Satisfiable"},
    {HttpStatus::INTERNAL_SERVER_ERROR, "Internal Server Error"},
    {HttpStatus::SERVICE_UNAVAILABLE, "Service Unavailable"},
    {HttpStatus::HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Suppported"}};

enum class HttpContentType {
    URLENCODED,
    MULTIPART,
    JSON,
    PLAIN,
    HTML,
    UNKNOW,
};

enum class HttpChunkedState {
    NO_LEN,
    NO_DATA,
    DONE,
    BAD,
};


inline const std::unordered_map<uint64_t, const char *> ContentType_Map{
    {"html"_h, "text/html"},
    {"htm"_h, "text/html"},
    {"css"_h, "text/css"},
    {"csv"_h, "text/csv"},
    {"txt"_h, "text/plain"},

    {"woff"_h, "font/woff"},
    {"woff2"_h, "font/woff2"},
    {"ttf"_h, "font/ttf"},
    {"otf"_h, "font/otf"},

    {"bmp"_h, "image/x-ms-bmp"},
    {"svg"_h, "image/svg+xml"},
    {"svgz"_h, "image/svg+xml"},
    {"gif"_h, "image/gif"},
    {"jpg"_h, "image/jpeg"},
    {"jpeg"_h, "image/jpeg"},
    {"webp"_h, "image/webp"},
    {"png"_h, "image/png"},
    {"ico"_h, "image/x-icon"},

    {"wav"_h, "audio/wav"},
    {"weba"_h, "audio/webm"},
    {"mp3"_h, "audio/mpeg"},
    {"oga"_h, "audio/ogg"},

    {"ogv"_h, "video/ogg"},
    {"mpeg"_h, "video/mpeg"},
    {"mp4"_h, "video/mp4"},
    {"avi"_h, "video/x-msvideo"},
    {"wmv"_h, "video/x-ms-wmv"},
    {"webm"_h, "video/webm"},
    {"flv"_h, "video/x-flv"},
    {"m4v"_h, "video/x-m4v"},

    {"7z"_h, "application/x-7z-compressed"},
    {"rar"_h, "application/x-rar-compressed"},
    {"sh"_h, "application/x-sh"},
    {"rss"_h, "application/rss+xml"},
    {"tar"_h, "application/x-tar"},
    {"jar"_h, "application/java-archive"},
    {"gz"_h, "application/gzip"},
    {"zip"_h, "application/zip"},
    {"xhtml"_h, "application/xhtml+xml"},
    {"xml"_h, "application/xml"},
    {"js"_h, "application/javascript"},
    {"pdf"_h, "application/pdf"},
    {"doc"_h, "application/msword"},
    {"xls"_h, "application/vnd.ms-excel"},
    {"ppt"_h, "application/vnd.ms-powerpoint"},
    {"eot"_h, "application/vnd.ms-fontobject"},
    {"json"_h, "application/json"},
    {"bin"_h, "application/octet-stream"},
    {"exe"_h, "application/octet-stream"},
    {"dll"_h, "application/octet-stream"},
    {"deb"_h, "application/octet-stream"},
    {"iso"_h, "application/octet-stream"},
    {"img"_h, "application/octet-stream"},
    {"dmg"_h, "application/octet-stream"},
    {"docx"_h,"application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {"xlsx"_h,"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {"pptx"_h,"application/""vnd.openxmlformats-officedocument.presentationml.presentation"}};


enum class HttpMethod { GET = 0x01, POST = 0x02, HEAD = 0x03 };


enum class HttpVersion {
    HTTP_1_0 = 0x10,
    HTTP_1_1 = 0x11,
    HTTP_2_0 = 0x20 /*not support HTTP 2.0*/
};


// 解析键值对格式数据
void parseKeyValue(const std::string& src, const std::string& join, const std::string& split,
                    HttpMap<CASE_SENSITIVE::YES> &map, bool urldecode = true);


// 表格类数据
class HttpForm : public HttpMap<CASE_SENSITIVE::YES>{

};

}

#endif