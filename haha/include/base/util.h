#ifndef __HAHA_UTIL_H__
#define __HAHA_UTIL_H__

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <vector>
#include <string>

namespace haha{

pid_t GetThreadId();

}

#endif