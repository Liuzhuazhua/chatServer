#ifndef USERMODEL_H
#define USERMODEL_H
#include "user.hpp"

//User表的数据操作类

class usermodel
{
private:
    /* data */
public:
    //user表的增加方法
    bool insert(User &user);//给业务层传递的都是字段
    //根据主键用户号码查询用户信息
    User query(int id);
    //更新用户状态信息
    void updatestate(User user);

    //重置用户的状态信息
    void resetState();

}



#endif