#ifndef __HAHA_HTTPMAP_H__
#define __HAHA_HTTPMAP_H__

#include <unordered_map>
#include "base/util.h"
#include "base/RetOption.h"

namespace haha{

enum class CASE_SENSITIVE{YES, NO};

/* 
CASE_SENSITIVE::YES 键区分大小写
CASE_SENSITIVE::NO 键不区分大小写
*/
template<typename K, typename V, CASE_SENSITIVE C = CASE_SENSITIVE::YES>
class HttpBaseMap{
public:
    typedef typename std::unordered_map<K, V>::iterator Iterator;
    typedef typename std::unordered_map<K, V>::const_iterator ConstIterator;

    HttpBaseMap():length_(0){}

    virtual void add(const K& key, const V& value){
        if(C == CASE_SENSITIVE::YES){
            map_.insert({key, value});
        }
        else{
            map_.insert({toLowers(key), value});
        } 
        length_ += key.size() + value.size();
    }

    virtual void del(const K& key){
        ConstIterator it;
        if(C == CASE_SENSITIVE::YES){
            it = map_.find(key);
        }
        else{
            it = map_.find(toLowers(key));
        }
        
        if(it != map_.end()){
            length_ -= key.size() + it->second.size();
            map_.erase(it);
        }
    }

    virtual bool contains(const K &key){
        if(C == CASE_SENSITIVE::YES){
            return map_.contains(key); 
        }
        return map_.contains(toLowers(key)); 
    }

    virtual RetOption<V> get(const K &key) const {
        ConstIterator it;
        if(C == CASE_SENSITIVE::YES){
            it = map_.find(key);
        }
        else{
            it = map_.find(toLowers(key));
        }
        
        if(it == map_.end()){
            return {V(), false};
        }
        return {it->second, true};
    }

    Iterator begin(){ return map_.begin(); }
    Iterator end(){ return map_.end(); }

    // 返回键值对数量
    size_t keyCount() const { return map_.size(); }

    // 返回所有内容的大小
    size_t size() const { return length_; }

    bool empty() const { return length_ > 0; }

protected:
    std::unordered_map<K, V> map_;
    size_t length_;
};

template<CASE_SENSITIVE C = CASE_SENSITIVE::YES>
using HttpMap = HttpBaseMap<std::string, std::string, C>;

}

#endif