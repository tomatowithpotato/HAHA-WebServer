#ifndef __HAHA_STRINGVIEW_H__
#define __HAHA_STRINGVIEW_H__

#include <cstring>
#include <string>
#include <exception>
#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <algorithm>

namespace haha{

class StringView{

friend std::ostream& operator<<(std::ostream& ostream, StringView view);

public:
    static const size_t npos = -1;

    StringView() = delete;
    StringView(const char* begin, const char* end):begin_(begin),end_(end){}
    StringView(const char* begin, size_t len):begin_(begin),end_(begin+len){}

    /* 获取以pos开头的子串 */
    StringView substr(size_t pos){
        const char *begin1 = begin_ + pos;
        if(begin1 > end_){
            throw std::out_of_range("StringView substr out of range");
        }
        return StringView(begin1, end_);
    }

    /* 获取以pos开头，长度为len的子串 */
    StringView substr(size_t pos, size_t len){
        const char *begin1 = begin_ + pos;
        if(begin1 > end_){
            throw std::out_of_range("StringView substr out of range");
        }
        const char *end1 = begin1 + len - 1;
        if(end1 > end_){
            throw std::out_of_range("StringView substr out of range");
        }
        return StringView(begin1, end1);
    }

    /* 去掉前面的部分 */
    void remove_prefix(size_t len){
        size_t length = size();
        begin_ = len > length ? end_ : begin_+len;
    }

    /* 去掉后面的部分 */
    void remove_suffix(size_t len){
        size_t length = size();
        end_ = len > length ? begin_ : end_-len;
    }

    /* 判断str是否是其前缀 有范围限制 */
    bool starts_with(const char *begin, const char *end){
        size_t i = 0;
        while(begin+i != end && i < size() && begin[i] == begin_[i]){
            ++i;
        }
        return i < size() ? false : true;
    }

    /* 判断str是否是其前缀 */
    bool starts_with(const char *str){
        size_t i = 0;
        while(str[i] != '\0' && i < size() && str[i] == begin_[i]){
            ++i;
        }
        return i < size() ? false : true;
    }

    /* 判断str是否是其后缀 有范围限制 */
    bool ends_with(const char *begin, const char *end){
        if(begin == end)return true;
        if(begin < end || size() == 0 || size() < end - begin)return false;
        int i = end - begin - 1;
        int j = size() - 1;
        while(i >= 0 && j >= 0 && begin[i] == begin_[j]){
            --i;
            --j;
        }
        return i >= 0 ? false : true;
    }

    /* 判断str是否是其后缀 */
    bool ends_with(const char *str){
        int i = strlen(str);
        if(i == 0)return true;
        if(size() == 0 || size() < i)return false;
        --i;
        int j = size() - 1;
        while(i >= 0 && j >= 0 && str[i] == begin_[j]){
            --i;
            --j;
        }
        return i >= 0 ? false : true;
    }

    size_t find(const char *str, size_t pos = 0);

    size_t find(const char *begin, const char* end, size_t pos = 0);

    int compare(size_t start, size_t len, const char *str){
        if(begin_ + len > end_){
            throw std::out_of_range("len out of range");
        }
        return strncmp(begin_+start, str, len);
    }
    

    inline size_t size() const {return static_cast<size_t>(end_ - begin_);}

    const char& operator [](size_t pos) const{
        if(begin_ + pos > end_){
            throw std::out_of_range("index out of range");
        }
        return begin_[pos];
    }
    
private:
    const char* begin_;
    const char* end_;
    size_t length_;
};

inline std::ostream& operator<<(std::ostream& ostream, StringView view){
    for(auto i = view.begin_; i != view.end_; ++i){
        ostream << *i;
    }
    return ostream;
}

}

#endif