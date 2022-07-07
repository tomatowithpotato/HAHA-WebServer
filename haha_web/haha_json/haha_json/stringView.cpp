#include "stringView.h"
#include <vector>

namespace haha
{

namespace json{

void get_next(StringView t, std::vector<int> &next)
{
    int i = 0, j = -1;
    next[0] = -1;
    while (i < (int)t.size() - 1)
    {
        if (j == -1 || t[i] == t[j])
        {
            next[++i] = ++j;
        }
        else
        {
            j = next[j];
        }
    }
}

int KMP_search(const StringView &s, const StringView &t)
{
    int i = 0, j = 0;
    std::vector<int> next(t.size());
    get_next(t, next);
    // 此处涉及-1和unsigned比较
    // 由于signed int的负数最高位是1，转换成unsigned int之后，就会变成一个很大的unsigned int型正数
    while (j == -1 || (i < (int)s.size() && j < (int)t.size())) 
    {
        if (j == -1 || s[i] == t[j])
        {
            i++;
            j++;
        }
        else
        {
            j = next[j];
        }
    }
    if (j == (int)t.size())
        return i - j;
    else
        return -1;
}


size_t StringView::find(const char *begin, const char* end, size_t pos){
    if(pos >= size()){
        throw std::out_of_range("StringView find out of range");
    }
    size_t len = end - begin;
    if(len * size() < 100){
        return std::search(begin_+pos, end_, begin, end) - begin_;
    }
    return KMP_search(*this, StringView(begin, end));
}

size_t StringView::find(const char *str, size_t pos){
    if(pos >= size()){
        throw std::out_of_range("StringView find out of range");
    }
    size_t len = strlen(str);
    if(len * size() < 100){
        return std::search(begin_+pos, end_, str, str+len) - begin_;
    }
    return KMP_search(*this, StringView(str, len));
}

}
    
} // namespace haha
