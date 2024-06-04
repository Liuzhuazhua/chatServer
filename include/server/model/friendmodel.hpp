#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H
#include "user.hpp"
#include <vector>
using namespace std;

//维护好友信息
class friendmodel
{
    public:
    //添加好友信息
    void insert(int userid,int friendid);
    //返回用户好友列表 friendid
    vector<User> query(int userid);
}


#endif