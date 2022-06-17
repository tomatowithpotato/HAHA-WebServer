#include "http/HttpServer.h"

int main(){
    haha::InetAddress address(9999);
    haha::HttpServer server;
    server.start(address);
    return 0;
}