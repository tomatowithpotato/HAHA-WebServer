#ifndef __HAHA_SINGLETON_H__
#define __HAHA_SINGLETON_H__

#include "base/noncopyable.h"
#include <memory>

namespace haha{

template<typename T>
class Singleton : noncopyable{

public:
    static T& getInstance(){
        static T instance_;
        return instance_;
    }
};

template<class T>
class SingletonPtr : noncopyable{
public:
    /**
     * @brief 返回单例智能指针
     */
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};

}

#endif