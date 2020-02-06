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

enum Connection_state
{
    STATE_INIT = 1,
    STATE_RECV_CHOICE,
    STATE_LOGIN,
    STATE_MODIFY1,
    STATE_MODIFY2,
    STATE_REGISTER,
    STATE_FINISH,
    STATE_WRONG
};

typedef shared_ptr<Channel> SP_Channel;

class Client
{
public:
    Client(EventLoop* loop, int connfd);
    ~Client();
    int get_fd_() {return connfd_; }
    SP_Channel get_channel() { return channel_; }
    EventLoop* get_loop() { return loop_; }
    void link_timer(shared_ptr<TimerNode>& timer){ timer_ = timer; }
    void seperate_timer();
    void new_conn();
    Connection_state get_state(){ return conn_state; }
private:
    int connfd_;
    string in_buffer_;
    string out_buffer_;
    shared_ptr<Channel> channel_;
    EventLoop* loop_;
    Connection_state conn_state;
    weak_ptr<TimerNode> timer_;
    int Login();
    int modify();
    void handle_read();
    void handle_write();
    void handle_conn();
};

#endif