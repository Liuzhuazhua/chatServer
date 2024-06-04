#ifndef  CHATSERVICE_H
#define  CHATSERVICE_H
#include  <functional>
#include <muduo/net/TcpConnection.h>
#include <map>
#include <unordered_map>
#include "3party/json.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include <mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;

using MsgHandler=std::function<void(const TcpConnectionPtr & conn,json& js,Timestamp)>
//单例模式 业务模块只要一个实例就行

//映射一个事件回调


//聊天服务器业务类
class chatservice
{
   public:
   //获取单例对象的接口函数
   static chatservice* instance();
   //登录业务
   void login(const TcpConnectionPtr & conn,json &js,Timestamp time);
   //处理注册业务
   void reg(const TcpConnectionPtr & conn,json &js,Timestamp time);
   //服务器异常，业务重置方法
   void reset();
   //一对一聊天业务
   void onechat(const TcpConnectionPtr & conn,json &js,Timestamp time);
   //添加好友业务
   void addfriend(const TcpConnectionPtr & conn,json &js,Timestamp time);
   void createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);
   void addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);
   void groupchat(const TcpConnectionPtr &conn,json &js,Timestamp time);
   //获取消息对应的处理器
   MsgHandler getHandler(int msgid);
   //处理客户端异常退出
   clientCloseEception(const TcpConnectionPtr &conn);


   private:
   //单例模式构造函数自由化
   chatservice();//唯一的实例

   //存储消息id和其对应的业务处理方法
   unordered_map <int, MsgHandler> _msgHandlermap;

   //存储在线用户的通信连接
   unordered_map<int,MsgHandler> _msgConnMap; //一定要注意线程安全

   //定义互斥锁，保证_msgConnMap的线程安全
   mutex _connmutex;

   //数据操作类对象
   usermodel _userModel;
   offlinemessagemodel _offlineMSModel;
   friendmodel _friendmodel;
   groupmodel _groupModel;
}



#endif
