#include "TcpServer.h"

int main(){
    haha::TcpServer server;
    haha::InetAddress address(9999);
    server.start(address);
    return 0;
}