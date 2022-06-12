/**
 * 
 * uuid.hpp
 * uuid generator
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-06-25
 */

#ifndef __HAHA_UUID_H__
#define __HAHA_UUID_H__

#include <stdio.h>
#include <stdint.h>
#include <string>

#include <uuid/uuid.h>

#include "base/Mutex.h"

#define GUID_LEN 64

namespace haha
{
namespace uuid
{

static SpinLock uuid_mutex_;

// 生成uuid的线程不安全版本
static std::string generate()
{
    char buf[GUID_LEN] = { 0 };

    uuid_t uu;   
    ::uuid_generate( uu );   

    int32_t index = 0;
    for (int32_t i = 0; i < 16; i++)
    {
        int32_t len = i < 15 ? 
            sprintf(buf + index, "%02X-", uu[i]) : 
            sprintf(buf + index, "%02X", uu[i]);
        if(len < 0 )
            return std::move(std::string(""));
        index += len;
    }

    return std::move(std::string(buf));
}

// 生成uuid的线程安全版本
static std::string generate_threadSafe(){
    char buf[GUID_LEN] = { 0 };

    uuid_t uu;
    {
        SpinLock::RallLock lock(uuid_mutex_); 
        ::uuid_generate( uu ); 
    }  

    int32_t index = 0;
    for (int32_t i = 0; i < 16; i++)
    {
        int32_t len = i < 15 ? 
            sprintf(buf + index, "%02X-", uu[i]) : 
            sprintf(buf + index, "%02X", uu[i]);
        if(len < 0 )
            return std::move(std::string(""));
        index += len;
    }

    return std::move(std::string(buf));
}

}
}

#endif