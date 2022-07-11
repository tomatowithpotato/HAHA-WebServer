#ifndef __HAHA_JSON_JsonBuffer_H__
#define __HAHA_JSON_JsonBuffer_H__

#include <string>
#include <cstring>
#include <fstream>
#include <filesystem>
#include "jsonError.h"

namespace haha
{

namespace json
{

class JsonBuffer {
public:
    static const int max_size = 4 * 1000 * 1000;

    JsonBuffer(const JsonBuffer &) = delete;
    JsonBuffer(JsonBuffer &&) = delete;
    JsonBuffer &operator=(const JsonBuffer &) = delete;
    JsonBuffer &operator=(JsonBuffer &&) = delete;

    JsonBuffer() {}
    JsonBuffer(const std::string str) { readString(str); }

    ~JsonBuffer() {
        if (sbuffer_){
            delete[] sbuffer_;
        }
    }

    bool readFile(const char *filePath){
        bool exists = std::filesystem::exists(filePath);
        if(!exists){
            return false;
        }
        int fsz = std::filesystem::file_size(filePath);
        if(fsz > max_size){
            throw JsonError("unsupport file size bigger than 4M !!!");
        }

        if (sbuffer_) {
            delete[] sbuffer_;
            sbuffer_ = nullptr;
        }

        std::ifstream ifs(filePath);
        __len = fsz;
        __ptr = -1;
        sbuffer_ = new char[fsz+1]{0};
        ifs.read(sbuffer_, fsz);
        return true;
    }

    void readString(const std::string &s) {
        if (sbuffer_) {
            delete[] sbuffer_;
            sbuffer_ = nullptr;
        }
        __len = s.length();
        __ptr = -1;
        sbuffer_ = new char[__len]{0};
        memcpy(sbuffer_, s.data(), __len * sizeof(char));
    }

    bool writeFile(const char *filePath){
        if(sbuffer_ == nullptr){
            return false;
        }
        std::ofstream ofs(filePath);
        ofs << sbuffer_;
        return true;
    }

    void writeString(std::string &s){
        s = sbuffer_;
    }

    char cur() {
        if (__ptr <= -1 || __ptr >= __len){
            return EOF;
        }
        return sbuffer_[__ptr];
    }
    char get() {
        if (++__ptr >= __len){
            return EOF;
        }
        return sbuffer_[__ptr];
    }
    char backc() {
        if (__ptr - 1 < 0){
            return EOF;
        }
        return sbuffer_[__ptr - 1];
    }

    void back() { --__ptr; }

    char peek() {
        if (__ptr + 1 >= __len){
            return EOF;
        }
        return sbuffer_[__ptr + 1];
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
        memset(sbuffer_, 0, __len * sizeof(char));
    }

    std::string_view to_stringview(){
        return std::string_view(sbuffer_, __len);
    }

private:
    char* sbuffer_ = nullptr;
    int __ptr = -1;
    int __len = 0;
};

} // namespace json


} // namespace haha


#endif