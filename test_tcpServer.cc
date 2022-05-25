#include "haha/include/TcpServer.h"
// #include "haha/include/Channel.h"
// #include "haha/include/Epoller.h"
// #include "haha/include/EventLoop.h"
// #include "TcpConnection.h"

int main(){
    haha::TcpServer server;
    haha::InetAddress address(9999);
    server.start(address);
    // haha::EventLoop loop;
    return 0;
}