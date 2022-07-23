#include "LoggerManager.h"

namespace haha
{
    
namespace log
{

// LoggerManager::LoggerManager() 
// {
//     sync_root_ = std::make_shared<SyncLogger>("sync_root");
//     sync_root_->addAppender(LogAppender::ptr(new FileSyncLogAppender(default_sync_log_file, default_roll_size)));

//     sync_stdout_ = std::make_shared<SyncLogger>("sync_stdout");
//     sync_stdout_->addAppender(LogAppender::ptr(new StdoutSyncLogAppender));

//     async_root_ = std::make_shared<AsyncLogger>("async_root", default_flush_interval);
//     async_root_->addAppender(LogAppender::ptr(new FileAsyncLogAppender(default_async_log_file, default_roll_size)));

//     loggers_[sync_root_->getName()] = sync_root_;
//     loggers_[async_root_->getName()] = async_root_;
//     loggers_[sync_stdout_->getName()] = sync_stdout_;
// }

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    static const bool log_open = haha::config::GET_CONFIG<bool>("log.open",true);
    if(log_open == false){
        return nullptr;
    }
    
    MutexType::RAIILock lock(mutex_);
    auto it = loggers_.find(name);
    if(it != loggers_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace log

} // namespace haha
