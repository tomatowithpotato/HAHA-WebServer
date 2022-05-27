#include "base/util.h"

namespace haha{

pid_t GetThreadId(){
    return syscall(SYS_gettid);
}

}