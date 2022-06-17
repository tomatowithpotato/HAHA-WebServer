#include "http/HttpSession.h"

namespace haha{

static const int INTERVAL = 180; // 三分钟会话过期

HttpSession::HttpSession()
    :status_(Status::New)
    ,id_(uuid::generate_threadSafe())
    ,interval_(INTERVAL){
}

RetOption<HttpSession::ptr> HttpSessionManager::getSession(const std::string &id) const{
    ReadWriteLock::RAIIReadLock lock(mutex_);
    auto it = sessions_.find(id);
    if(it == sessions_.end()){
        return {nullptr, false};
    }
    return {it->second, true};
}

void HttpSessionManager::addSession(const std::string &id, HttpSession::ptr session){
    ReadWriteLock::RAIIWriteLock lock(mutex_);
    sessions_[id] = session;
}

HttpSession::ptr HttpSessionManager::newSession(){
    HttpSession::ptr session = std::make_shared<HttpSession>();
    ReadWriteLock::RAIIWriteLock lock(mutex_);
    sessions_[session->getId()] = session;
    return session;
}

void HttpSessionManager::delSession(const std::string &id){
    ReadWriteLock::RAIIWriteLock lock(mutex_);
    sessions_.erase(id);
}

}