#include <iostream>
#include <string>
#include <mysql.h>
#include "mysql++.h"
#include "ssqls.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::setw;
using std::ios;
using namespace mysqlpp;

static const char* database = "USERS";
static const char* server = "localhost";
static const char* user = "root";
static const char* password = "123456";
mysqlpp::Connection conn(false);

sql_create_2(user_info, 1, 2,
    mysqlpp::sql_char, username,
    mysqlpp::sql_char, password
    )
enum Process_state
{
    state_init = 1,
    //state_login,
    //state_ready_modify,
    state_modify,
    //state_register,
    state_finish
};

enum Connection_state
{
    Connected = 1,
    Disconnecting,
    Disconnected
};

void display_mysql()
{
    mysqlpp::Query query = conn.query("select * from user_info");
    /*
    mysqlpp::Query query = conn.query();
    query << "select * from user_info";
    query.execute();
    */
    if(mysqlpp::StoreQueryResult res = query.store())
    {
        cout.setf(ios::left);
        cout << setw(31) << "username " << setw(10) << "password" << endl;
        for(auto it = res.begin(); it != res.end(); ++it)
        {
            mysqlpp::Row row = *it;
            cout << setw(31) << row[0] << setw(10) << row[1] << endl;
        }
    } 
    else 
        cout << "query() failed" << endl;
}
void init_mysql()
{
//连接数据库
    if(conn.connect(database, server, user, password))
    {
        cout << "mysql connect succeed" << endl;
//打印数据库当前数据
        /*
        mysqlpp::Query query = conn.query("select * from user_info");
        query << "select * from user_info" << endl;
        query.execute();
        if(mysqlpp::StoreQueryResult res = query.store())
        {
            cout << res.size() << endl;
            cout.setf(ios::left);
            cout << setw(31) << "username " << setw(10) << "password" << endl;
            for(auto it = res.begin(); it != res.end(); ++it)
            {
                mysqlpp::Row row = *it;
                cout << setw(31) << row[0] << setw(10) << row[1] << endl;
            }
        } 
        else 
            cout << "query() failed" << endl;
        */
    }
    else 
    {
        cout << "mysql connect failed" << endl;
    }
}
//检查用户名和密码是否正确
bool Check(const string& username, const string& password)
{
    //mysqlpp::Query query = conn.query("select password from user_info where username= '" + username + "'");
    mysqlpp::Query query = conn.query();
    query << "select * from user_info where username = " << mysqlpp::quote << username;
    //query.execute();
    mysqlpp::StoreQueryResult res = query.store();
    int row_num = res.size();
    cout << row_num << endl;
    if(row_num != 1)
    {
        cout << "wrong username" << endl;
        return false;
    }
    else
    {
        auto row = res.begin();
        if((*row)[1] == password)
        {
            cout << "valid user" << endl;
            return true;
        }
        else
        {
            cout << "invalid user" << endl;
            return false;
        }
    }
    return false;
}
//向数据库中注册新用户
void Register(const string& username, const string& password)
{
    user_info new_user(username, password);
    mysqlpp::Query query = conn.query();
    query.insert(new_user);
    cout << "Query: " << query << endl;
    query.execute();
}
//修改用户密码
void Modify(const string& username, const string& new_password)
{
    mysqlpp::Query query = conn.query("select * from user_info");
    query << " where username = " << mysqlpp::quote << username;
    StoreQueryResult res = query.store();
    //cout << res.size() << endl;
    if(res)
    {
        user_info row = res[0];
        user_info origin_row = row;
        row.password = new_password;
        query.update(origin_row, row);
        //cout << "Query: " << query << endl;
        query.execute();
    }
}
int main()
{
//初始化连接状态和处理状态
    Process_state process_state = state_init;
    Connection_state connection_state = Connected;
    init_mysql();

    while(true)
    {
        display_mysql();
        if(process_state == state_init)
        {
            if(connection_state == Connected)
            {
                cout << "Please input your operation: " << endl;
                string choice;
                cin >> choice;

                if(choice == "L" || choice == "l")
                {
                    cout << "Please input your username and password: " << endl;
                    string username, password;
                    cin >> username >> password;
                    if(!Check(username, password))
                    {
                        cout << "invalid user" << endl;
                        process_state = state_init;
                    }
                    else 
                    {
                        process_state = state_finish;
                    }
                }
                else if(choice == "M" || choice  == "m")
                {
                    process_state = state_modify;
                    cout << "Please input your username and password: " << endl;
                    string username, password;
                    cin >> username >> password;
                    if(!Check(username, password))
                    {
                        cout << "In modify_ready, invalid user or password" << endl;
                        process_state = state_finish;
                    }
                    else 
                    {
                        cout << "Please input your new password" << endl;
                        string new_password;
                        cin >> new_password;
                        
                        Modify(username, new_password);
                    
                        process_state = state_init;
                        continue;
                    }
                }
                else if(choice == "R" || choice == "r")
                {
                    //process_state = state_register;
                    string username, password;
                    while(1)
                    {
                        cout << "Register your username and password: " << endl;
                        cin >> username >> password;
                        mysqlpp::Query query = conn.query("select * from user_info");
                        query << " where username = " << mysqlpp::quote << username;
                        StoreQueryResult res = query.store();
                        if(res.size() != 0)
                        {
                            cout << "username already exists, please input again." << endl;
                            continue;
                        }
                        else
                            break;
                    }
                    Register(username, password);
                }
                else
                {
                    cout << "invalid choice, disconnecting......" << endl;
                    connection_state = Disconnecting;
                    continue;
                }
            }
    //暂时将Disconnecting和Disconnected一起处理
            else if(connection_state == Disconnecting || connection_state == Disconnected)
            {
                //结束当前函数
                return 0;
            }
        }
        if(process_state == state_modify)
        {
            if(connection_state == Connected)
            {
                cout << "Modify your username and new password: " << endl;
                string username, new_password;
                cin >> username >> new_password;
                Modify(username, new_password);
                process_state = state_finish;
                continue;
            }
            else if(connection_state == Disconnecting || connection_state == Disconnected)
            {
                //结束当前函数
                return 0;
            }
        }
        if(process_state == state_finish)
        {
            if(connection_state == Disconnecting || connection_state == Disconnected)
            {
                return 0;
            }
            else
            {
                cout << "Would you try again...YES or NO" << endl;
                string try_again;
                cin >> try_again;
                if(try_again == "y" || try_again == "Y")
                {
                    process_state = state_init;
                    continue;
                }
                else 
                {
                    break;
                }
            }
        }
    }
    return 0;
}