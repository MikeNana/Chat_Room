#include "Epoll.h"
#include <iostream>

using std::cout;
using std::endl;


const int MAX_EVENTS = 1000;
const int DEFAULT_EVENT = EPOLLIN | EPOLLHUP | EPOLLPRI;
const int EPOLL_WAIT_TIME = 10000;

Epoller::Epoller()
    :
    epoll_fd_(epoll_create1(EPOLL_CLOEXEC)),
    ready_events(MAX_EVENTS)
{}

void Epoller::epoll_add(shared_ptr<Channel> channel_, int timeout)
{
    int cur_fd = channel_->get_fd();
    shared_ptr<Client> client_ = channel_->get_holder();
//向管理器中添加新定时器节点
    if(timeout > 0)
    {
        timer_manager_.add_timer(client_, timeout);
    }
//向轮询器中添加新的事件
    struct epoll_event ev;
    ev.events = DEFAULT_EVENT;
    ev.data.fd = cur_fd;
    if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, cur_fd, &ev) < 0)
    {
        perror("epoll add failed");
        return;
    }
//向记录中添加新成员
    fd_channel[cur_fd] = channel_;
    fd_client[cur_fd] = client_;
}

void Epoller::epoll_mod(shared_ptr<Channel> channel_, int timeout)
{
    int cur_fd = channel_->get_fd();
    shared_ptr<Client> client_ = channel_->get_holder();
//向管理器中添加新定时器节点
    if(timeout > 0)
    {
        timer_manager_.add_timer(client_, timeout);
    }
//向轮询器中添加新的事件
    struct epoll_event ev;
    ev.events = channel_->get_events();
    ev.data.fd = cur_fd;
    if(epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, cur_fd, &ev) < 0)
    {
        perror("epoll modify failed");
        return;
    }
    fd_client[cur_fd] = client_;
}

void Epoller::epoll_del(shared_ptr<Channel> channel_)
{
    int cur_fd = channel_->get_fd();
    shared_ptr<Client> client_ = channel_->get_holder();
    client_->seperate_timer();
    fd_client[cur_fd].reset();
    fd_channel[cur_fd].reset();
}

void Epoller::add_timer(shared_ptr<Channel> channel_, int timeout)
{
    shared_ptr<Client> client_ = channel_->get_holder();
    timer_manager_.add_timer(client_, timeout);
}

//处理超时节点，将节点从队列中弹出，交给定时管理器处理
void Epoller::handle_expired_events()
{
    timer_manager_.handle_expired_events();
}

int Epoller::get_epoll_fd_()
{
    return epoll_fd_;
}

vector<shared_ptr<Channel>> Epoller::poll()
{
    while(true)
    {
        int ready_channel_num = epoll_wait(epoll_fd_, &*ready_events.begin(), ready_events.size(), EPOLL_WAIT_TIME);
        vector<shared_ptr<Channel>> req_channel = get_ready_events(ready_channel_num);
        if(!req_channel.empty())
            return req_channel;
    }
}

vector<shared_ptr<Channel>> Epoller::get_ready_events(int ready_channel_num)
{
    vector<shared_ptr<Channel>> req_channel;
    
    //cout << ready_channel_num << endl;

    for(int i = 0; i < ready_channel_num; ++i)
    {
        int cur_fd = ready_events[i].data.fd;
        shared_ptr<Channel> cur_channel = fd_channel[cur_fd];
        if(cur_channel)
        {
            cur_channel->set_revents(ready_events[i].events);
            cur_channel->set_events(0);
            req_channel.emplace_back(cur_channel);
        }
        else 
        {
            cout << "wrong ready channel" << endl;
        }
    }
    return req_channel;
}