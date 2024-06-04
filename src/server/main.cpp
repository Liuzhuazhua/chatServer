
#include <iostream>
#include "chatserver.hpp"
#include <signal.h>
using namespace std;

//处理服务器ctrl+c结束后，重置
void resetHandler(int)
{
    chatservice::instance()->reset();
    exit(0);
}
int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1",6000);
    chatserver server(&loop,addr,"ChatServer");

    server.start();
    loop.loop();

    return 0;
}
