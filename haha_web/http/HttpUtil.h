#ifndef __HAHA_HTTPUTIL_H__
#define __HAHA_HTTPUTIL_H__

#include <unordered_map>
#include <string>
#include "base/util.h"
#include "base/RetOption.h"
#include "http/HttpMap.h"
#include "base/EncodeUtil.h"

#include <random>
#include <time.h>

namespace haha{

// inline constexpr uint64_t hashExt(const char *s, size_t i, size_t l,
//                                   uint64_t h) noexcept {
//   return i == l ? h : hashExt(s, i + 1, l, (h * 31) + *(s + i));
// }
// inline uint64_t hashExt(const std::string_view &s) noexcept {
//   return hashExt(s.data(), 0, s.size(), 0);
// }
// inline constexpr uint64_t operator""_h(const char *s, size_t n) noexcept {
//   return hashExt(s, 0, n, 0);
// }

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

inline const std::unordered_map<short, std::string> HttpStatus2Str = {
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

enum class HttpChunkedState {
    NO_LEN,
    NO_DATA,
    DONE,
    BAD,
};


enum class HttpContentType {
    URLENCODED,
    MULTIPART,
    JSON,
    PLAIN,
    HTML,
    JPG,
    JPEG,
    PNG,
    BMP,
    AVI,
    MP4,
    WEBM,
    UNKNOW,
};


inline const std::unordered_map<HttpContentType, std::string> HttpContentType2Name {
    {HttpContentType::URLENCODED, "application/x-www-form-urlencoded"},
    {HttpContentType::MULTIPART, "multipart/form-data"},
    {HttpContentType::PLAIN, "text/plain"},
    {HttpContentType::HTML, "text/html"},
    {HttpContentType::JSON, "application/json"},
    {HttpContentType::JPEG, "image/jpeg"},
    {HttpContentType::JPG, "image/jpg"},
    {HttpContentType::PNG, "image/png"},
    {HttpContentType::BMP, "image/bmp"},
    {HttpContentType::AVI, "video/x-msvideo"},
    {HttpContentType::MP4, "video/mp4"},
    {HttpContentType::WEBM, "video/webm"},
    {HttpContentType::JSON, "application/json"},
};

inline const std::unordered_map<std::string, HttpContentType> Name2HttpContentType(
  [](){
    std::unordered_map<std::string, HttpContentType> temp;
    for(auto &[k, v] : HttpContentType2Name){
        temp[v] = k;
    }
    return temp;
  }()
);


inline const std::unordered_map<std::string, std::string> Ext2HttpContentTypeName{
    {".html", "text/html"},
    {".htm", "text/html"},
    {".css", "text/css"},
    {".csv", "text/csv"},
    {".txt", "text/plain"},

    {".woff", "font/woff"},
    {".woff2", "font/woff2"},
    {".ttf", "font/ttf"},
    {".otf", "font/otf"},

    {".bmp", "image/x-ms-bmp"},
    {".svg", "image/svg+xml"},
    {".svgz", "image/svg+xml"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".webp", "image/webp"},
    {".png", "image/png"},
    {".ico", "image/x-icon"},

    {".wav", "audio/wav"},
    {".weba", "audio/webm"},
    {".mp3", "audio/mpeg"},
    {".oga", "audio/ogg"},

    {".ogv", "video/ogg"},
    {".mpeg", "video/mpeg"},
    {".mp4", "video/mp4"},
    {".avi", "video/x-msvideo"},
    {".wmv", "video/x-ms-wmv"},
    {".webm", "video/webm"},
    {".flv", "video/x-flv"},
    {".m4v", "video/x-m4v"},
    
    {".7z", "application/x-7z-compressed"},
    {".rar", "application/x-rar-compressed"},
    {".sh", "application/x-sh"},
    {".rss", "application/rss+xml"},
    {".tar", "application/x-tar"},
    {".jar", "application/java-archive"},
    {".gz", "application/gzip"},
    {".zip", "application/zip"},
    {".xhtml", "application/xhtml+xml"},
    {".xml", "application/xml"},
    {".js", "application/javascript"},
    {".pdf", "application/pdf"},
    {".doc", "application/msword"},
    {".xls", "application/vnd.ms-excel"},
    {".ppt", "application/vnd.ms-powerpoint"},
    {".eot", "application/vnd.ms-fontobject"},
    {".json", "application/json"},
    {".bin", "application/octet-stream"},
    {".exe", "application/octet-stream"},
    {".dll", "application/octet-stream"},
    {".deb", "application/octet-stream"},
    {".iso", "application/octet-stream"},
    {".img", "application/octet-stream"},
    {".dmg", "application/octet-stream"},
    {".docx","application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {".xlsx","application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {".pptx","application/""vnd.openxmlformats-officedocument.presentationml.presentation"}};
  

inline const std::unordered_map<std::string, HttpContentType> Ext2HttpContentType(
    [](){
        std::unordered_map<std::string, HttpContentType> temp;
        for(auto &[k, v] : Ext2HttpContentTypeName){
            auto it = Name2HttpContentType.find(v);
            if(it != Name2HttpContentType.end()){
                temp[k] = it->second;
            }
        }
        return temp;
    }()
);

inline const std::unordered_map<HttpContentType, std::string> HttpContentType2Ext(
    [](){
        std::unordered_map<HttpContentType, std::string> temp;
        for(auto &[k, v] : Ext2HttpContentType){
            temp[v] = k;
        }
        // 因为HTML既对应html也对应htm，这里就只对应html
        temp[HttpContentType::HTML] = "html";
        return temp;
    }()
);


enum class HttpMethod { GET = 0x01, POST = 0x02, HEAD = 0x03 };


enum class HttpVersion {
    HTTP_1_0 = 0x10,
    HTTP_1_1 = 0x11,
    HTTP_2_0 = 0x20 /*not support HTTP 2.0*/
};

inline const std::unordered_map<HttpMethod, std::string> HttpMethod2Str{
    {HttpMethod::GET, "GET"},
    {HttpMethod::POST, "POST"},
    {HttpMethod::HEAD, "HEAD"},
};

inline const std::unordered_map<HttpVersion, std::string> HttpVersion2Str{
    {HttpVersion::HTTP_1_0, "HTTP/1.0"},
    {HttpVersion::HTTP_1_1, "HTTP/1.1"},
    {HttpVersion::HTTP_2_0, "HTTP/2.0"},
};


// 解析键值对格式数据
void parseKeyValue(const std::string& src, const std::string& join, const std::string& split,
                    HttpMap<CASE_SENSITIVE::YES> &map, bool urldecode = true);


// 表格类数据
class HttpForm : public HttpMap<CASE_SENSITIVE::YES>{

};

}

#endif