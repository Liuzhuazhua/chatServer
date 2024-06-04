#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace std;
using namespace muduo;
using namespace muduo::net;


//聊天服务器的主类
class chatserver
{
private:
   TcpServer _sever;//组合的muduo库，实现服务器功能的类对象
   EventLoop *_loop;//指向事件循环对象的指针

   //上报链接相关信息的回调函数
   void onConnection(const TcpConnectionPtr&);
   
   //上报读写事件相关信息的回调函数
   void onMessage(const TcpConnectionPtr&,
                        Buffer*,
                        Timestamp)
public:
    //初始化聊天服务器对象
    chatserver::chatserver(EventLoop* loop,
        const InetAddress& listenAddr,
        const string&nameArg);
    //启动服务
    void start;
};



#endif