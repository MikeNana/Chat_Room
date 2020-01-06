#include "Channel.h"
#include <functional>
#include <sys/epoll.h>
#include <memory>
#include <iostream>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::weak_ptr;

Channel::Channel(EventLoop* loop, int connfd)
    :
    connfd_(connfd),
    loop_(loop),
    events_(0),
    revents_(0)
{}

Channel::Channel(EventLoop* loop)
    :
    connfd_(0),
    loop_(loop),
    events_(0),
    revents_(0)
{}
//该函数有问题
shared_ptr<Client> Channel::get_holder()
{
    if(holder_.lock())
    {
        shared_ptr<Client> ret(holder_.lock());
        return ret;
    }
    return nullptr;
}
void Channel::handle_event()
{
//因为是ET触发，所以需要将events置0
    events_ = 0;
    if((revents_ & EPOLLHUP) && (revents_ & EPOLLIN))
    {
        events_ = 0;
        return;
    }
    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLHUP))
    {
        if(read_handler_)
        {
            cout << "reading..." << endl;
            read_handler_();
        }
            
    }
    if(revents_& EPOLLOUT)
    {
        if(write_handler_)
        {
            cout << "writing..." << endl;
            write_handler_();
        }
    }
    else 
    {
        if(conn_handler_)
            conn_handler_();
    }
}

void Channel::set_readhandler(CALLBACK&& read_handler)
{
    read_handler_ = read_handler;
}

void Channel::set_writehandler(CALLBACK&& write_handler)
{
    write_handler_ = write_handler;
}

