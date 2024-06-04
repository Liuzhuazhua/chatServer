#include "offlinemessagemodel.hpp"
#include "db.h"


void offlinemessagemodel::insert(int userid,string msg)
{
    //1.组装sql语句
    char sql[1024]={0};
    sprintf(sql,"insert into offlinemessage values(%d,'%s')",userid,msg.c_str());

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
            
    }
}

void offlinemessagemodel::remove(int userid)
{
    //1.组装sql语句
    char sql[1024]={0};
    sprintf(sql,"delete from offlinemessage where id = %d",userid);

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
            
    }
}


vector<string> offlinemessagemodel::query(int userid)
{
    //1.组装sql语句
    char sql[1024]={0};
    sprintf(sql,"select * from offlinemessage where userid = %d",userid);

    vector<string> vec;

    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES *res=mysql.query(sql);
        if(res!=nullptr)
        {

            //把user用户的所有离线消息放入vec中返回
            MYSQL_ROW row;
            while ((row =mysql_fetch_row(res))!=nullptr)
            {
                vec.push_back(row[0]);

            }
            mysql_free_result(res);//释放资源
            return vec;
        }
            
    }
    return vec;
    
}