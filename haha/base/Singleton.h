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

}

#endif