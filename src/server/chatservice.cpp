#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/logging.h>
using namespace muduo;
chatservice* chatservice::instance()
{
    static chatservice service;
    return &service;
}

chatservice::chatservice()
{
    _msgHandlermap.insert({LOGIN_MSG,std::bind(&chatservice::login,this,_1,_2,_3)});
    _msgHandlermap.insert({REG_MSG,std::bind(&chatservice::reg,this,_1,_2,_3)});
    _msgHandlermap.insert({ONE_CHAT_MSG,std::bind(&ChatService::oneChat,this,_1,_2,_3)});
    _msgHandlermap.insert({ADD_FRIEND_MESSAGE,std::bind(&ChatService::addfriend,this,_1,_2,_3)});
}

//登录业务  ORM 业务层操作的都是对象 DAO 数据层对数据库操作 id pwd 
void chatservice::login(const TcpConnectionPtr & conn,json &js,Timestamp time)
{
    int id = js["id"].get<int>();
    string pwd =js["password"];

    User user =_userModel.query(id);//传入一个id值返回id值对应的数据
    if(user.getId()==id && user.getPwd()==pwd)
    {
        if(user.getState()=="online")
        {
            //已登录不能重复登录
            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errno"]=2;
            response["errmsg"]="该账号已登录不能重复登录，请输入新账号";
            conn->send(response.dump());
        }
        else{
            //登录成功，记录用户连接信息
            {
                lock_guard<mutex> lock(_connmutex);
                 _userConnMap.insert({id,conn});
            }
            //{}代表作用域
            //登录成功，更新用户状态
            user.setState("online");
            _userModel.updatestate(user);
            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errno"]=0;
            response["id"]=user.getId();
            response["name"]=user.getName();

            //查询该用户是否有离线消息
            vector<string> vec=_offlineMSModel.query(id);
            if(!vec.empty())
            {
                response["offlinemsg"]=vec;
                _offlineMSModel.remove(id);

            }
            //查询该用户的好友信息
            vector<User> userVec =_friendmodel.query(id);
            if(!userVec.empty())
            {
                vector<string> vec2;
                for(User& user:userVec)
                {
                    json js;
                    js["id"]=user.getId();
                    js["name"]=user.getName();
                    js["state"]=user.getState();
                    vec2.push_back(js.dump());
                }
            }


            conn->send(response.dump());
        }  

    }
    else{
        //登录失败
        json response;
        response["msgid"]=LOGIN_MSG_ACK;
        response["errno"]=1;
        response["errmsg"]="用户名或密码错误";
        conn->send(response.dump());
    }

}
//处理注册业务  name password 
void chatservice::reg(const TcpConnectionPtr & conn,json &js,Timestamp time)
{
    string name =js["name"];
    string pwd =js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state =_userModel.insert(user);
    if(state)
    {
        //注册成功
        json response;
        response["msgid"]=REG_MSG_ACK;
        response["errno"]=0;
        response["id"]=user.getId();
        conn->send(response.dump());

    }
    else 
    {
        //注册失败
        json response;
        response["msgid"]=REG_MSG_ACK;
        response["errno"]=1;
        conn->send(response.dump());

    }
}
//服务器异常，业务重置方法
void chatservice::reset()
{
    //把所有online状态用户设置成offline
    _offlineMSModel.resetState();
}

//获取消息对应的处理器
MsgHandler getHandler(int msgid)
{
    //记录错误日志，msgid没有对应的事件处理回调
    auto it=_msgHandlermap.find(msgid);
    if(it== _msgHandlermap.end())
    { 
        //LOG_ERROR<<"msgid:"<<msgid<<"can not find handler!";
        //返回一个默认的处理器空操作
        return [=](const TcpConnectionPtr & conn,json& js,Timestamp){
            LOG_ERROR<<"msgid:"<<msgid<<"can not find handler!";
        };
        
    }
    else  {return _msgHandlermap[msgid];}
    
}

void chatservice::clientCloseEception(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connmutex);
        for (auto it = _userConnMap.begin(); it!=_userConnMap.end(); ++it)
        {
            if(it->second == conn)
            {
                    //从map表中删除用户的链接消息
                    user.setId(it->first);
                    _userConnMap.erase(it);
                    break;
            }
        }
    }
    //更新用户的状态信息
    if(user.getId()!= -1)
    {
        user.setState("offline");
        _userModel.updatestate(user);

    }
}
//点对点聊天业务
void ChatService::onechat(const TcpConnectionPtr & conn,json &js,Timestamp time);
{
    int toid =js["to"].get<int>();
    bool userstate=_userConnMap.find(toid);

    {
        lock_guard<mutex> lock(_connmutex);
        auto it =_userConnMap.find(toid);
        if(it != _userConnMap.end())
        {
            //toid在线，转发消息 服务器主动推送消息给toid用户
            it->second->send(js.dump());
            return;

        }
    }
    //toid不在线，存储离线消息

    _offlineMSModel.insert(toid,js.dump());


}

//添加好友业务
void chatservice::addfriend(const TcpConnectionPtr & conn,json &js,Timestamp time)
{
    int userid =js["id"].get<int>();
    int friend =js["friendid"].get<int>();

    //存储好友信息
    _friendModel.insert(userid,friendid);

}

void chatservice::createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid =js["id"].get<int>();
    string name =js["groupname"];
    string desc =js["groupdesc"];

    //存储新创建的群组信息
    Group group(-1,name,desc);
    if(_groupModel.createGroup(group))
    {
        _groupModel.addGroup(userid,group.getId(),"creator");
    }

}

void chatservice::addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid =js["id"].get<int>();
    int groupid  =js["groupid"].get<int>();

    //存储好友信息
    _groupModel.addGroup(userid,groupid,"normal");

}

void chatservice::groupchat(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid =js["id"].get<int>();
    int groupid  =js["groupid"].get<int>();

   vector<int> useridvec=_groupModel.queryGroupUsers(userid,groupid);
   for(int id:useridvec)
   {
    lock_guard<mutex> lock(_connMutex);
    auto it  =_userConnMap.find(id);
    if(it !=_userConnMap.end())
    {
        if->second->send(js.dump());

    }
    else{
        _offlineMsgModel.insert(id,js.dump());
    }
   } 
}
