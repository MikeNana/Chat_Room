//服务器头文件
//内含一个主循环，接受新连接的channel，一个线程池

#ifndef _Server_H_
#define _Server_H_

#include <memory>
#include <set>
#include "EventLoopThreadPool.h"
#include "Channel.h"

using std::set;
using std::shared_ptr;
using std::vector;
using std::unique_ptr;

static const int port = 8000;

class EventLoop;

class Server
{
private:
    bool started_;
    bool quit_;
    int thread_nums_;
    int listen_fd_;
//用于存放已连接文件描述符集合
//  set<int> conn_fds_;
//由于new传回的对象是指针形式，所以这里用智能指针管理线程池
    unique_ptr<EventLoopThreadPool> thread_pool_;
//server_loop_会管理连接事件的到来
    EventLoop* server_loop_;
    shared_ptr<Channel> accept_channel_;
public:
    Server(int thread_num, EventLoop* server_loop);
    ~Server();
    int start();
//处理新连接
    void handle_newconn();
};

#endif