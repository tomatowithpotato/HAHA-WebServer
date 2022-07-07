#ifndef __HAHA_HTTPCOOKIE_H__
#define __HAHA_HTTPCOOKIE_H__

#include <string>
#include "HttpUtil.h"

namespace haha{

class HttpCookie : public HttpMap<CASE_SENSITIVE::NO>{
public:
    HttpCookie(const std::string &str = "");
    std::string toString() const;

    void setExpires(const std::string &expires) { expires_ = expires; }
    const std::string &getExpires() const noexcept { return expires_; }
    void setDomain(const std::string &domain) { domain_ = domain; }
    const std::string &getDomain() const noexcept { return domain_; }
    void setPath(const std::string &path) { path_ = path; }
    const std::string &getPath() const noexcept { return path_; }
    void setMaxAge(int second) { maxAge_ = second; }
    int getMaxAge() const noexcept { return maxAge_; }
    void setSessionId(const std::string &ssid) { sessionId_ = ssid; }
    const std::string &getSessionId() const { return sessionId_; }

    void setSecure(bool secure) { secure_ = secure; }
    bool isSecure() const noexcept { return secure_; }
    void setHttpOnly(bool httponly) { httpOnly_ = httponly; }
    bool isHttpOnly() const noexcept { return httpOnly_; }
private:
    std::string expires_;
    std::string domain_;
    std::string path_;
    std::string sessionId_;
    int maxAge_;
    bool secure_;
    bool httpOnly_;
};

}

#endif