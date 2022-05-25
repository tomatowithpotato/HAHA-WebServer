#ifndef __HAHA_SINGLETON_H__
#define __HAHA_SINGLETON_H__

#include "base/noncopyable.h"

namespace haha{

template<typename T>
class SingleTon : noncopyable{

public:
    T* getInstance(){
        static T instance_;
        return &instance_;
    }
    
};

}

#endif