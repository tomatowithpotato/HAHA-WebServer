#ifndef __HAHA_HTTPSERVER_H__
#define __HAHA_HTTPSERVER_H__

#include "TcpServer.h"
#include "HttpRequest.h"

namespace haha{

class HttpServer : public TcpServer{
protected:
    MESSAGE_STATUS onMessage(TcpConnection::ptr) override;
    bool onNewConntection(TcpConnection::ptr) override;
    bool onCloseConntection(TcpConnection::ptr) override;

    void addGET();
    void addPOST();
};

}

#endif