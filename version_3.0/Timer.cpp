#include "Timer.h"
#include <ctime>
#include <sys/time.h>


TimerNode::TimerNode(shared_ptr<Client> client, int timeout)
    :
    client_(client)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    expired_time_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

TimerNode::~TimerNode()
{}

int TimerNode::get_expired_time()
{
    return expired_time_;
}

Timer_Manager::Timer_Manager()
{}

Timer_Manager::~Timer_Manager()
{}
//增加时间节点并连接定时器
void Timer_Manager::add_timer(shared_ptr<Client> client, int timeout)
{
    shared_ptr<TimerNode> tmp(new TimerNode(client, timeout));
    timer_queue.emplace(tmp);
    client->link_timer(tmp);
}

//处理超时事件
void Timer_Manager::handle_expired_events()
{
    while(!timer_queue.empty())
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        shared_ptr<TimerNode> cur_node = timer_queue.top();
        if((((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) < cur_node->get_expired_time())
            break;
        else
            timer_queue.pop();
    }
}