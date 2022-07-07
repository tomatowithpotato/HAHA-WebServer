#ifndef __HAHA_JSON_JSONREADER_H__
#define __HAHA_JSON_JSONREADER_H__

#include <string>
#include <cstring>
#include <fstream>
#include <filesystem>
#include "jsonError.h"

namespace haha
{

namespace json
{

class JsonReader {
public:
    static const int max_size = 4 * 1000 * 1000;

    JsonReader(const JsonReader &) = delete;
    JsonReader(JsonReader &&) = delete;
    JsonReader &operator=(const JsonReader &) = delete;
    JsonReader &operator=(JsonReader &&) = delete;

    JsonReader() {}
    explicit JsonReader(const std::string &str) { readString(str); }

    ~JsonReader() {
        if (__sbuffer){
            delete[] __sbuffer;
        }
    }

    int readFile(const char *filePath){
        bool exists = std::filesystem::exists(filePath);
        if(!exists){
            return false;
        }
        int fsz = std::filesystem::file_size(filePath);
        if(fsz > max_size){
            throw JsonError("unsupport file size bigger than 4M !!!");
        }

        if (__sbuffer) {
            delete[] __sbuffer;
            __sbuffer = nullptr;
        }

        std::ifstream ifs(filePath);
        __len = fsz;
        __ptr = -1;
        __sbuffer = new char[fsz+1]{0};
        ifs.read(__sbuffer, fsz);
        return true;
    }

    void readString(const std::string &s) {
        if (__sbuffer) {
            delete[] __sbuffer;
            __sbuffer = nullptr;
        }
        __len = s.length();
        __ptr = -1;
        __sbuffer = new char[__len]{0};
        memcpy(__sbuffer, s.data(), __len * sizeof(char));
    }

    char cur() {
        if (__ptr <= -1 || __ptr >= __len){
            return EOF;
        }
        return __sbuffer[__ptr];
    }
    char get() {
        if (++__ptr >= __len){
            return EOF;
        }
        return __sbuffer[__ptr];
    }
    char backc() {
        if (__ptr - 1 < 0){
            return EOF;
        }
        return __sbuffer[__ptr - 1];
    }

    void back() { --__ptr; }

    char peek() {
        if (__ptr + 1 >= __len){
            return EOF;
        }
        return __sbuffer[__ptr + 1];
    }

    std::pair<bool, char> geteq(int c) {
        char x = get();
        if (x == c){
            return std::make_pair(true, c);
        }
        back();
        return std::make_pair(false, c);
    }

    std::string gets(int count) {
        std::string s;
        for (int i = 0; i < count; i++){
            s.append(1, get());
        }
        return s;
    }

    int len() { return __len; }
    int pos() { return __ptr; }
    bool eof() { return __ptr > __len - 1; }

    void reset() { __ptr = -1; }

    void clear() {
        reset();
        memset(__sbuffer, 0, __len * sizeof(char));
    }

    std::string_view to_stringview(){
        return std::string_view(__sbuffer, __len);
    }

private:
    char *__sbuffer = nullptr;
    int __ptr = -1, __len = 0;
};

} // namespace json


} // namespace haha


#endif