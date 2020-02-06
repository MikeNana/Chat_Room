#include "Client.h"
#include "Channel.h"
#include "Helper.h"
#include "EventLoop.h"
#include <functional>
#include <memory>
#include <sys/unistd.h>
#include <iostream>
#include <sys/epoll.h>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <mysql.h>
#include "mysql++.h"
#include "ssqls.h"

using std::set;
using std::shared_ptr;
using std::bind;
using std::cout;
using std::endl;
using std::unique_lock;
using std::ios;
using std::setw;

using namespace mysqlpp;

extern set<int> link_pool;
const int MAX_BUFF = 1000;
const char* u_a_p = "Please input your username and password: ";
const char* invalid_user = "Wrong username or password, continue...";
char read_buf[500];
char write_buf[500];

static const char* database = "USERS";
static const char* server = "localhost";
static const char* user = "root";
static const char* password = "123456";
mysqlpp::Connection conn(false);

sql_create_2(user_info, 1, 2,
    mysqlpp::sql_char, username,
    mysqlpp::sql_char, password
    )

void seperate_username_and_password(string& un, string& pw, const string& uap, int length)
{
    int pos = 0;
    while(uap[pos] != ' ')
    {
        un += uap[pos];
        ++pos;
    }
    ++pos;
    while(pos != length-1)
    {
        pw += uap[pos];
        ++pos;
    }
    return;
}
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

Client::Client(EventLoop* loop, int connfd)
    :
    connfd_(connfd),
    loop_(loop),
    channel_(new Channel(loop_, connfd_)),
    conn_state(STATE_INIT)
{
    channel_->set_readhandler(bind(&Client::handle_read, this));
    channel_->set_writehandler(bind(&Client::handle_write, this));
    channel_->set_connhandler(bind(&Client::handle_conn, this));
}

Client::~Client(){}

void Client::seperate_timer()
{
    if(timer_.lock())
    {
        shared_ptr<TimerNode> SP_Timer(timer_.lock());
        //SP_Timer->clean_req();
        timer_.reset();
    }
}

//原版，不加mysql用户管理模块
/*
void Client::handle_read()
{
    cout << "from loop " << get_loop()->get_epollfd_() << endl;
    ssize_t read_len = 0;
    bool zero = false;
    EventLoop* loop_ = get_loop();
    
    read_len = read_str(connfd_, loop_->buffer_, zero);
    cout << loop_->buffer_.size() << endl;
    cout << read_len << endl;
    if(read_len < 0)
    {
        handle_error("read failed");
    }
//客户端主动close/ctrl+c/kill
    else if(read_len == 0)
    {
        cout << "client shutdown." << endl;
        pause();
    }
//具体格式后面修改
    cout << loop_->buffer_ << endl;
}
*/

void Client::handle_read()
{
    if(conn_state == STATE_INIT)
    {
        //cout << "aaaaaaaaaaaaaaaaaaaaaaa" << endl;
        /*
        string greet = "Please input your choice: ";
        write_str(connfd_, greet);
        */
        bool zero = false;
        ssize_t read_len = 0;
        int length = 0;
        read_len = read_str(connfd_, in_buffer_, zero, length);
        if(read_len < 0)
        {
            handle_error("read failed");
        }
        else if(read_len == 0)
        {
            cout << "client shutdown." << endl;
            pause();
        }
        cout << in_buffer_ << endl;
        in_buffer_ = in_buffer_.substr(0, 1);
        cout << in_buffer_;
        cout << length << endl;
        if((length == 2) && (in_buffer_[0] == 'l' || in_buffer_[0] == 'L'))
        {
            conn_state = STATE_LOGIN;
            in_buffer_.clear();
            //cout << "bbbbbbbbbbbbbbbbbbbbbbbbbbb" << endl;
        }
        else if((length == 2) && (in_buffer_[0] == 'm' || in_buffer_[0] == 'M'))
        {
            conn_state = STATE_MODIFY1;
            in_buffer_.clear();
        }
        else if((length == 2) && (in_buffer_[0] == 'r' || in_buffer_[0] == 'R'))
        {
            conn_state = STATE_REGISTER;
            in_buffer_.clear();
        }
        else 
            conn_state = STATE_WRONG;
        in_buffer_.clear();
        return;
    }
    if(conn_state == STATE_LOGIN)
    {
        //check the username and password
        string username, password, username_and_password;
        bool zero = false;
        int length = 0;
        ssize_t read_len = 0;
        read_len = read_str(connfd_, in_buffer_, zero, length);
//解析username和password，然后开始验证
        username_and_password = in_buffer_;
        seperate_username_and_password(username, password, username_and_password, length);
        cout << "username:" << username << " length: " << username.size() << endl;
        cout << "password:" << password << " length: " << password.size() << endl;
        
        in_buffer_.clear();
        //read_len = read_str(connfd_, in_buffer_, zero, length);
        //password = in_buffer_;
        
        //cout << "password:" << password << endl;

        init_mysql();
        if(Check(username, password))
        {
            conn_state = STATE_FINISH;
            link_pool.insert(connfd_);
            //cout << "valid user, login success" << endl;
        }
        else
        {
            conn_state = STATE_INIT;
            //cout << "invalid user, login failed" << endl;
        }
        return;
    }
    if(conn_state == STATE_MODIFY1)
    {
        //check the username and password
        string username, password, username_and_password;
        bool zero = false;
        int length = 0;
        ssize_t read_len = 0;
        read_len = read_str(connfd_, in_buffer_, zero, length);
//解析username和password，然后开始验证
        username_and_password = in_buffer_;
        seperate_username_and_password(username, password, username_and_password, length);
        cout << "username:" << username << " length: " << username.size() << endl;
        cout << "password:" << password << " length: " << password.size() << endl;
        
        in_buffer_.clear();
        //read_len = read_str(connfd_, in_buffer_, zero, length);
        //password = in_buffer_;
        
        //cout << "password:" << password << endl;

        init_mysql();
        if(Check(username, password))
        {
            conn_state = STATE_MODIFY2;
            //cout << "valid user, login success" << endl;
        }
        else
        {
            conn_state = STATE_INIT;
            //cout << "invalid user, login failed" << endl;
        }
        return;
    }
    if(conn_state == STATE_MODIFY2)
    {
        //check the username and password
        string username, new_password, username_and_password;
        bool zero = false;
        int length = 0;
        ssize_t read_len = 0;
        read_len = read_str(connfd_, in_buffer_, zero, length);
//解析username和password，然后开始验证
        username_and_password = in_buffer_;
        seperate_username_and_password(username, new_password, username_and_password, length);
        cout << "username:" << username << " length: " << username.size() << endl;
        cout << "new password:" << new_password << " length: " << new_password.size() << endl;
        
        in_buffer_.clear();
        //read_len = read_str(connfd_, in_buffer_, zero, length);
        //password = in_buffer_;
        
        //cout << "password:" << password << endl;

        init_mysql();
        Modify(username, new_password);
        conn_state = STATE_INIT;
        return;
    }
    if(conn_state == STATE_REGISTER)
    {
        //check the username and password
        string username, password, username_and_password;
        bool zero = false;
        int length = 0;
        ssize_t read_len = 0;
        read_len = read_str(connfd_, in_buffer_, zero, length);
//解析username和password，然后开始验证
        username_and_password = in_buffer_;
        seperate_username_and_password(username, password, username_and_password, length);
        cout << "username:" << username << " length: " << username.size() << endl;
        cout << "password:" << password << " length: " << password.size() << endl;
        
        in_buffer_.clear();
        //read_len = read_str(connfd_, in_buffer_, zero, length);
        //password = in_buffer_;
        
        //cout << "password:" << password << endl;

        init_mysql();
        Register(username, password);
        conn_state = STATE_INIT;
        return;
    }
    if(conn_state == STATE_FINISH)
    {
        //cout << "ccccccccccccccccccccccc" << endl;
        cout << "from loop " << get_loop()->get_epollfd_() << endl;
        ssize_t read_len = 0;
        int length = 0;
        bool zero = false;
        EventLoop* loop_ = get_loop();
        
        read_len = read_str(connfd_, loop_->buffer_, zero, length);
        if(read_len < 0)
        {
            handle_error("read failed");
        }
    //客户端主动close/ctrl+c/kill
        else if(read_len == 0)
        {
            cout << "read done" << endl;
        }
    //具体格式后面修改
        cout << loop_->buffer_ << endl;
    }
    if(conn_state == STATE_WRONG)
    {
        cout << "wrong state" << endl;
        conn_state = STATE_INIT;
    }
}
void Client::handle_write()
{
    ssize_t write_len = 0;
    if((write_len = write_str(connfd_, out_buffer_)) < 0)
    {
        handle_error("write failed");
    }
//后面改具体格式
    cout << out_buffer_ << endl;
}

void Client::handle_conn()
{
    /*
    const char* choice = "Please input your choice: ";
    if(conn_state == STATE_INIT)
    {
        if(write(connfd_, choice, sizeof(choice)) < 0)
        {
            conn_state = STATE_WRONG;
        }
        else
        {
            conn_state = STATE_RECV_CHOICE;
        }
    }
    if(conn_state == STATE_RECV_CHOICE)
    {
        if(read(connfd_, read_buf, 500) < 0)
        {
            conn_state = STATE_WRONG;   
        }
        else
        {
            string choice(read_buf);
            if(choice == "L" || choice == "l")
                conn_state = STATE_LOGIN;
            else if(choice == "M" || choice == "m")
                conn_state = STATE_MODIFY;
            else if(choice == "R" || choice == "r")
                conn_state = STATE_REGISTER;
            else 
                conn_state = STATE_WRONG;
        }
    }
    if(conn_state == STATE_LOGIN)
    {
        if(write(connfd_, u_a_p, sizeof(u_a_p)) < 0)
            conn_state = STATE_WRONG;
        else 
        {
//从客户端读取客户端输入的username和password
            if(read(connfd_, read_buf, 500 < 0))
                conn_state = STATE_WRONG;
            else
            {
                string username(read_buf);
                if(read(connfd_, read_buf, 500 < 0))
                    conn_state = STATE_WRONG;
                else
                {
                    string password(read_buf);
                    init_mysql();
                    if(Check(username, password))
                    {
                        conn_state = STATE_FINISH;
                    }
                    else
                    {
                        write(connfd_, invalid_user, sizeof(invalid_user));
                        conn_state = STATE_INIT;
                    }
                }
                    
            }
        }
    }
    if(conn_state == STATE_MODIFY)
    {

    }
    if(conn_state == STATE_REGISTER)
    {

    }
    if(conn_state == STATE_FINISH)
    {
        read_len = read_str(connfd_, loop_->buffer_, zero);
    }
    */
    seperate_timer();
    loop_->handle_curconn(channel_);
}
void Client::new_conn()
{
    channel_->set_events(EPOLLIN | EPOLLET);
    loop_->handle_newconn(channel_, 0);
}
