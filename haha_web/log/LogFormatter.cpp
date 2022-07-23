#include "log/LogFormatter.h"
#include <map>
#include <functional>
#include <iostream>

namespace haha{

namespace log{

/* *************************************LogFormatter 重量级************************************* */

LogFormatter::LogFormatter(const std::string &pattern)
    :m_pattern(pattern){
    init();
}


// %xxx %xxx{xxx} %%
void LogFormatter::init(){
    // str, format, type
    // type 0 普通字符串 1 格式化字符串
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr; // 普通字符串
    for(size_t i = 0; i < m_pattern.size(); i++){
        if(m_pattern[i] != '%'){
            nstr.append(1, m_pattern[i]);
            continue;
        }
        // %% -> %
        if(i + 1 < m_pattern.size()){
            if(m_pattern[i+1] == '%'){
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str; // % 到 { 之间的字符串
        std::string fmt; // { 到 } 之间的字符串
        while(n < m_pattern.size()){
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')){
                str = m_pattern.substr(i+1, n-i-1);
                break;
            }
            if(fmt_status == 0){
                if(m_pattern[n] == '{'){
                    str = m_pattern.substr(i+1, n-i-1);
                    fmt_status = 1; // 解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }    
            }
            else if(fmt_status == 1){
                if(m_pattern[n] == '}'){
                    fmt = m_pattern.substr(fmt_begin+1, n-fmt_begin-1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()){
                if(str.empty()){
                    str = m_pattern.substr(i+1);
                }
            }
        }

        if(fmt_status == 0){
            // 无格式普通字符串
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            // 格式字符串
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        }
        // 未转到状态0，解析出错
        else if(fmt_status == 1){
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }
    if(!nstr.empty()){
        vec.push_back(std::make_tuple(nstr, std::string(), 0));
    }

    // %m -- 消息体
    // %p -- level
    // %r -- 启动后的时间
    // %c -- 日志名称
    // %t -- 线程id
    // %n -- 回车换行
    // %d -- 时间
    // %f -- 文件名
    // %l -- 行号
    // %T -- tab

    // 建立格式符到相应的格式处理器的映射
    static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string &fmt){return FormatItem::ptr(new C(fmt));}}
        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElapseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIdFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FilenameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        XX(T, TabFormatItem),               //T:Tab
        // XX(F, FiberIdFormatItem),           //F:协程id
        XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };
    for(auto &i : vec){
        // 普通字符串 直接放入
        if(std::get<2>(i) == 0){
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }
        else{
            // 格式字符串 将格式符映射为对应的格式处理器
            auto it = s_format_items.find(std::get<0>(i));
            // 找不到就加报错信息
            if(it == s_format_items.end()){
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            }
            else{
                // 找到就加入
                m_items.push_back(FormatItem::ptr(it->second(std::get<1>(i))));
            }
        }
        // // 控制台打印
        // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
}


void LogFormatter::format(outStream &ss, const LogInfo &info){
    for(auto &i : m_items){
        i->format(ss, info);
    }
}

// LogFormatter::outStream LogFormatter::format(LogInfo::ptr info){
//     outStream ss;
//     for(auto &i : m_items){
//         i->format(ss, info);
//     }
//     return ss;
// }

}

}