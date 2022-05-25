#ifndef __HAHA_HTTPSERVER_H__
#define __HAHA_HTTPSERVER_H__

#include "TcpServer.h"

namespace haha{

class HttpServer : TcpServer{
public:
    bool onMessage(TcpConnection *) override;
    bool onNewConntection(TcpConnection *) override;
    bool onCloseConntection(TcpConnection *) override;

    void addGET();
    void addPOST();
};

}

#endif