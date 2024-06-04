#include "chatserver.hpp"
#include "chatservice.hpp"
#include <functional>
#include  "json.hpp"
using namespace placeholders;
using json=nlohmann::json;
 chatserver::chatserver(EventLoop* loop,
        const InetAddress& listenAddr,
        const string&nameArg)
    : _server(loop,listenAddr,nameArg),_loop(loop)
{
    //注册来链接回调
    _server.setConnectionCallback(std::bind(&chatserver::onConnection,this,_1));

    //注册消息回调
    _server.setMessageCallback(std::bind(&chatserver::onMessage,this,_1,_2,_3));

    //设置线程数量
    _server.setThreadNum(4);


}

void chatserver::start()
{
    _server.start();
}

void chatserver::onConnection(const TcpConnectionPtr &)
{
    //客户端断开连接
    if(!conn->connected())
    {
        chatservice::instance();
        conn->shutdown();
    }

}
void chatserver::onMessage(const TcpConnectionPtr&,
                        Buffer*,
                        Timestamp)
{
    string buf =buffer->receiveAllAsString();
    //数据的反序列化
    json js =json::parse(buf);

    //目的：完全解耦网络模块的代码和业务模块的代码
    //通过js["msgid"]绑定回调操作，获取=》业务处理器handler=》 conn  js 
    //完全解耦的两种方法：1.接口编程，C++中即抽象类 2.回调操作实现

    auto msghandler=chatservice::instance()->genHandler(js["msgid"].get<int>());//js[""].get<int>()将json强转成int型
    //回调消息绑定好的事件处理器，来执行相应的业务处理
    msghandler(conn,js,time);
}