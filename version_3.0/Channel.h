//channel类似于事件
//事件就绪都是通过channel来回调

#ifndef _Channel_H_
#define _Channel_H_


#include "Client.h"
#include <functional>
#include <memory>

using std::function;
using std::shared_ptr;
using std::weak_ptr;

class EventLoop;
class Client;

class Channel
{
private:
    typedef function<void()> CALLBACK; 
    int connfd_;
    int events_;
    int revents_;
    EventLoop* loop_;
    weak_ptr<Client> holder_;

    CALLBACK read_handler_;
    CALLBACK write_handler_;
    CALLBACK conn_handler_;

public: 
    Channel(EventLoop* loop, int connfd);
    Channel(EventLoop* loop);
    ~Channel(){}
    
    int get_fd(){ return connfd_; }
    shared_ptr<Client> get_holder();
    int get_events(){ return events_; }
    int get_revents(){ return revents_; }
    void set_events(int events){ events_ = events; }
    void set_revents(int revents){ revents_ = revents; }
    void handle_event();
    void set_readhandler(CALLBACK&& read_handler){ read_handler_ = read_handler; }
    void set_writehandler(CALLBACK&& write_handler){ write_handler_ = write_handler; }
    void set_connhandler(CALLBACK&& conn_handler){ conn_handler_ = conn_handler; }
    //void set_connhandler(CALLBACK&& conn_handler);

    void set_fd(int connfd){ connfd_ = connfd; }
    void set_holder(shared_ptr<Client> holder){ holder_ = holder; }
};

#endif