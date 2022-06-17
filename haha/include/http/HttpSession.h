#ifndef __HAHA_HTTPSESSION_H__
#define __HAHA_HTTPSESSION_H__

#include <string>
#include <unordered_map>
#include <memory>
#include <any>
#include "base/noncopyable.h"
#include "base/ReadWriteLock.h"
#include "http/HttpMap.h"
#include "base/RetOption.h"
#include "base/uuid.h"

namespace haha{

class HttpSession : public HttpBaseMap<std::string, std::any>{
public:
    typedef std::shared_ptr<HttpSession> ptr;
    HttpSession();
    enum Status { New, Accessed, Destroy };
    const std::string& getId() const { 
        ReadWriteLock::RAIIReadLock lock(mutex_);
        return id_; 
    }
    bool isNew() const { 
        ReadWriteLock::RAIIReadLock lock(mutex_);
        return status_ == Status::New; 
    }
    bool isAccessed() const { 
        ReadWriteLock::RAIIReadLock lock(mutex_);
        return status_ == Status::Accessed; 
    }
    bool isDestroy() const { 
        ReadWriteLock::RAIIReadLock lock(mutex_);
        return status_ == Status::Destroy; 
    }
    int getMaxInactiveInterval() const { 
        ReadWriteLock::RAIIReadLock lock(mutex_);
        return interval_; 
    }

    void setStatus(Status status) { 
        ReadWriteLock::RAIIWriteLock lock(mutex_);
        status_ = status; 
    }

    template<typename V>
    void setValue(const std::string& key, const V& val){
        ReadWriteLock::RAIIWriteLock lock(mutex_);
        add(key, std::forward<const V &>(val));
    }

    template<typename V>
    const V* getValue(const std::string& key) const {
        ReadWriteLock::RAIIReadLock lock(mutex_);
        auto ret = get(key);
        if(!ret.exist()){
            return nullptr;
        }
        try{
            return &(std::any_cast<const V&>(ret.value()));
        }
        catch(std::bad_any_cast &){
            
        }
        return nullptr;
    }

private:
    std::string id_;
    int interval_;
    Status status_;

    mutable ReadWriteLock mutex_;
};


class HttpSessionManager : public noncopyable{
public:
    static HttpSessionManager& getInstance(){
        static HttpSessionManager manger;
        return manger;
    }
    RetOption<HttpSession::ptr> getSession(const std::string &id) const;
    void addSession(const std::string &id, HttpSession::ptr session);
    HttpSession::ptr newSession();
    void delSession(const std::string &id);
private:
    mutable ReadWriteLock mutex_;
    std::unordered_map<std::string, HttpSession::ptr> sessions_;
};

}

#endif