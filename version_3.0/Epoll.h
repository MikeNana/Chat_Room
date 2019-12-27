//Header file for epoller
//function:
//1.封装epoll_add/epoll_mod/epoll_del
//2.add_timer功能，将新来的连接作为timerNode加入到timerqueue中
//3.处理超时连接的接口，调用timermanager的对应函数来处理

#pragma once
#include <vector>
#include <memory>
#include <unordered_set>
#include <sys/epoll.h>
#include "Timer.h"
#include "Msgdata.h"

using std::vector;

class Channel;
class Msgdata;

class Epoller
{
public:
    typedef std::shared_ptr<Channel> SP_Channel;
    typedef std::shared_ptr<Msgdata> SP_Msgdata;
    Epoller();
    ~Epoller();
//轮询开始，会返回就绪事件，然后在各自的eventloop中分别处理
    vector<epoll_event> poll();
    vector<epoll_event> get_ready_events();
//对底层EPOLL_CTL_ADD/MOD/DEL的封装
    void epoll_add(SP_Channel message, int timeout);
    void epoll_mod(SP_Channel message, int timeout);
    void epoll_del(SP_Channel message);
//由上层的eventloop调用add_timer将主线程轮询到的新的事件添加到timer_manager中
    void add_timer(SP_Channel new_customer, int timeout);
    int get_epoll_fd();
    void handle_Expired();

private:
//该聊天室最多容纳一千人
    static const int MAXFDS = 1000;
    int epoll_fd;
    vector<epoll_event> events_;
    vector<SP_Channel> fd_sp_channel;
    vector<SP_Msgdata> fd_sp_msgdata;
    Timer_Manager timer_manager;

};