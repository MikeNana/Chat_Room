#include "Channel.h"
#include "Helper.h"
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
//如果对端正常关闭(程序里close(),shell下kill或ctrl+c)
    if((revents_ & EPOLLRDHUP) && (revents_ & EPOLLIN))
    {
        cout << "client exited." << endl;
        if(conn_handler_)
        {
            conn_handler_();
            return;
        }
    }
//如果对端有数据可读，正常发送数据(包括带外数据)
    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLHUP))
    {
        if(read_handler_)
        {
            cout << "reading..." << endl;
            read_handler_();
            return;
        }
    }
//如果有数据要写
    if(revents_& EPOLLOUT)
    {
        if(write_handler_)
        {
            cout << "writing..." << endl;
            write_handler_();
            return;
        }
    }
//如果是服务器端出错，注意EPOLLERR是在服务器端向已经关闭的socket写数据时才会触发的
    if((revents_ & EPOLLERR) || (revents_ & EPOLLHUP))
    {
        cout << "server error occured." << endl;
        return;
    }
    else 
    {
        cout << "other situation" << endl;
        system("pause");
        if(conn_handler_)
            conn_handler_();
    }
}



