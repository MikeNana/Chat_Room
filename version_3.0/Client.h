//Client类
//包含了与消息相关的消息:Channel, EventLoop, TimerNode, connfd_以及输入输出缓冲区

#ifndef _Client_H_
#define _Client_H_

#include "Timer.h"
#include "Channel.h"
#include <string>
#include <memory>
#include <functional>

using std::function;
using std::shared_ptr;
using std::weak_ptr;
using std::string;

class EventLoop;
class Channel;
class TimerNode;

typedef shared_ptr<Channel> SP_Channel;

class Client
{
public:
    Client(EventLoop* loop, int connfd);
    ~Client();
    int get_fd() {return connfd_; }
    SP_Channel get_channel() { return channel_; }
    EventLoop* get_loop() { return loop_; }
    void link_timer(shared_ptr<TimerNode>& timer){ timer_ = timer; }
    void seperate_timer();
    void new_conn();
private:
    int connfd_;
    string in_buffer_;
    string out_buffer_;
    shared_ptr<Channel> channel_;
    EventLoop* loop_;
    
    weak_ptr<TimerNode> timer_;
    void handle_read();
    void handle_write();
    void handle_conn();
};

#endif