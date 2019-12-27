// Header file for message coming from client
//function:
//1.初次到来的连接会连接相关的定时器节点
//2.节点超时的处理函数
//3.底层处理读写的函数
#include "Channel.h"
#include "EventLoop.h"
#include "Timer.h"
#include <memory>

using std::shared_ptr;

class TimerNode;
class Channel;
class EventLoop;

class Msgdata
{
public:
    typedef shared_ptr<Channel> SP_Channel;
    typedef shared_ptr<EventLoop> SP_EventLoop;
    Msgdata(SP_EventLoop eventloop_, int connfd_);
    ~Msgdata();
    void reset();
    void link_timer(shared_ptr<TimerNode> timer_node_);
    void seperate_timer_node();
    SP_EventLoop get_eventloop();
    SP_Channel get_channel();
    int get_fd();
//对应timernode超时，调用其析构函数时调用该函数来处理后续的事
    void handle_close();
//该连接第一次到来时需要处理的内容，包括设置concern_events，添加到epoller
    void new_comer();
private:
//管理该Msg_data的几个类
    SP_Channel channel;
    SP_EventLoop eventloop;
    TimerNode timer_node;
    int connfd;
    void handle_read();
    void handle_write();
    void handle_error();
};