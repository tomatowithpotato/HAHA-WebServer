#include "Socket.h"

namespace haha{

void sockops::setNonBlocking(int fd) {
    int old = ::fcntl(fd, F_GETFL, 0);
    old |= O_NONBLOCK;
    ::fcntl(fd, F_SETFL, old);
}

void sockops::setNoDelay(int fd) {
    int opt = 1;
    ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
}

int sockops::createSocket() {
    return ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC,
                  IPPROTO_TCP);
}

int sockops::createNBSocket() {
  return ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                  IPPROTO_TCP);
}

InetAddress sockops::getPeerName(int fd) {
    sockaddr_in sockaddrIn;
    socklen_t len = sizeof(sockaddrIn);
    ::getpeername(fd, (sockaddr *)&sockaddrIn, &len);
    InetAddress address(sockaddrIn);
    return address;
}

InetAddress sockops::getSockName(int fd) {
    sockaddr_in sockaddrIn;
    socklen_t len = sizeof(sockaddrIn);
    ::getsockname(fd, (sockaddr *)&sockaddrIn, &len);
    InetAddress address(sockaddrIn);
    return address;
}


std::shared_ptr<SSL_CTX> createSslCtx(const std::string &certfile, 
                                        const std::string &pkfile,
                                        const std::string &password)
{
    std::shared_ptr<SSL_CTX> ctx = nullptr;
    ctx.reset(::SSL_CTX_new(TLS_server_method()), SSL_CTX_free);
    if (!ctx) {
        HAHA_LOG_ERROR(HAHA_LOG_ASYNC_FILE_ROOT()) << "createSslCtx faild !!!";
        ::ERR_print_errors_fp(stderr);
        ::exit(-1);
    }
    ::SSL_CTX_set_options(ctx.get(),
                            SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 |
                            SSL_OP_NO_COMPRESSION |
                            SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
    
    ::SSL_CTX_set_default_passwd_cb_userdata(
    ctx.get(), password.empty() ? nullptr : (void *)password.c_str());

    if (::SSL_CTX_use_certificate_file(ctx.get(), certfile.c_str(),
                                        SSL_FILETYPE_PEM) <= 0) {
        ::ERR_print_errors_fp(stderr);
        ::exit(-1);
    }
    if (::SSL_CTX_use_PrivateKey_file(ctx.get(), pkfile.c_str(), SSL_FILETYPE_PEM) <=
        0) {
        ::ERR_print_errors_fp(stderr);
        ::exit(-1);
    }
    if (::SSL_CTX_check_private_key(ctx.get()) <= 0) {
        ::ERR_print_errors_fp(stderr);
        ::exit(-1);
    }

    return ctx;
}


Socket::Socket(FDTYPE fdtype){
    if(fdtype == FDTYPE::BLOCK){
        fd_ = sockops::createSocket();
        isBlocked_ = true;
    }
    else if(fdtype == FDTYPE::NONBLOCK){
        fd_ = sockops::createNBSocket();
        isBlocked_ = false;
    }
}

Socket::~Socket() { ::close(fd_); }

void Socket::bind(const InetAddress &address) {
    if (::bind(fd_, address.getSockAddr(), sizeof(sockaddr_in)) < 0)
        ::exit(-1);
}

void Socket::listen() {
    if (::listen(fd_, SOMAXCONN) < 0)
        ::exit(-1);
}

Socket::ptr Socket::accept() {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int connfd = ::accept(fd_, (sockaddr *)&addr, &len);
    if(connfd <= 0){
        return nullptr;
    }
    return std::make_shared<Socket>(connfd);
}

void Socket::setNonBlocking(){
    isBlocked_ = false;
    sockops::setNonBlocking(fd_);
}

void Socket::enableReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::enableReusePort(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::enableKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

void Socket::ignoreSIGPIPE(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, MSG_NOSIGNAL, &optval, sizeof(optval));
}

InetAddress Socket::getLocalAddress(){
    return sockops::getSockName(fd_);
}

InetAddress Socket::getRemoteAddress(){
    return sockops::getPeerName(fd_);
}

int Socket::send(Buffer::ptr buff, int *lastLen){
    int sendedBytes = 0;
    int saveErrno;
    int len;
    do {
        len = buff->WriteFd(fd_, &saveErrno);
        if (len <= 0) {
            break;
        }
        sendedBytes += len;
    } while (!isBlocked_ && buff->ReadableBytes());
    if(lastLen){
        *lastLen = len;
    }
    return sendedBytes;
}

int Socket::recv(Buffer::ptr buff, int *lastLen){
    int receivedBytes = 0;
    int saveErrno;
    int len;
    do {
        len = buff->ReadFd(fd_, &saveErrno);
        if (len <= 0) {
            break;
        }
        receivedBytes += len;
    } while (!isBlocked_);
    if(lastLen){
        *lastLen = len;
    }
    return receivedBytes;
}

/* *************************************SSLsocket************************************* */

namespace {

struct _SSLInit {
    _SSLInit() {
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
    }
};

// 全局初始化SSL的一些配置
static const _SSLInit s_init;

}

SslSocket::SslSocket(int fd, std::shared_ptr<SSL_CTX> ctx)
    :Socket(fd),
    ctx_(ctx)
{
    ssl_ = SSL_new(ctx_.get());
    SSL_set_fd(ssl_, fd_);
}

SslSocket::~SslSocket(){
    SSL_free(ssl_);
}


Socket::ptr SslSocket::accept(){
    if(ctx_ == nullptr){
        HAHA_LOG_ERROR(HAHA_LOG_ASYNC_FILE_ROOT()) << "bad accpet, please loadCertificate at first !!!";
        return nullptr;
    }
    int newfd = ::accept(fd_, nullptr, nullptr);
    if(newfd == -1){
        return nullptr;
    }
    auto sock = std::make_shared<SslSocket>(newfd, ctx_);
    return sock;
}


int SslSocket::send(Buffer::ptr buff, int *lastLen){
    int sendedBytes = 0;
    int saveErrno;
    int len;
    do {
        len = buff->WriteSsl(ssl_, &saveErrno);
        if (len <= 0) {
            break;
        }
        sendedBytes += len;
    } while (!isBlocked_ && buff->ReadableBytes());
    if(lastLen){
        *lastLen = len;
    }
    return sendedBytes;
}


int SslSocket::recv(Buffer::ptr buff, int *lastLen){
    int receivedBytes = 0;
    int saveErrno;
    int len;
    do {
        len = buff->ReadSsl(ssl_, &saveErrno);
        if (len <= 0) {
            break;
        }
        receivedBytes += len;
    } while (!isBlocked_);
    if(lastLen){
        *lastLen = len;
    }
    return receivedBytes;
}

}