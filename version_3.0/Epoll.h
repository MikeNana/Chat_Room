//轮询器


#ifndef _Epoll_H_
#define _Epoll_H_

#include "Channel.h"
#include "Client.h"
#include "Timer.h"
#include <vector>
#include <memory>
#include <sys/epoll.h>

using std::vector;
static const int MAXFDS = 1000;

class EventLoop;

class Epoller
{
private:
    int epoll_fd_;
    EventLoop* loop_;
    Timer_Manager timer_manager_;
    shared_ptr<Client> fd_client[MAXFDS];
    shared_ptr<Channel> fd_channel[MAXFDS];
    vector<epoll_event> ready_events;
public:
    Epoller();
    ~Epoller(){}

    void epoll_add(shared_ptr<Channel> channel_, int timeout);
    void epoll_mod(shared_ptr<Channel> channel_, int timeout);
    void epoll_del(shared_ptr<Channel> channel_);
    void add_timer(shared_ptr<Channel> channel_, int timeout);
    void handle_expired_events();

    int get_epoll_fd_();

    vector<shared_ptr<Channel>> poll();
    vector<shared_ptr<Channel>> get_ready_events(int ready_channel_num);
};

#endif