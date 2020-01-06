//定时器头文件
//内含定时器节点/定时器管理器两个类；每个连接会有一个对应的定时器节点，由管理器管理
//管理器的容器选择优先队列，如果超时则抛弃

#ifndef _Timer_H_
#define _Timer_H_

#include "Client.h"
#include <memory>
#include <queue>

using std::priority_queue;
using std::shared_ptr;

class Client;

class TimerNode
{
private:
    int expired_time_;
    shared_ptr<Client> client_;
public:
    explicit TimerNode(shared_ptr<Client> client, int timeout);
    ~TimerNode();
    int get_expired_time();
};


struct TimerNode_cmp
{   
    bool operator()(const shared_ptr<TimerNode> tn1, const shared_ptr<TimerNode> tn2)
    {
        return tn1->get_expired_time() < tn2->get_expired_time();
    }
};
class Timer_Manager
{
private:
    priority_queue<shared_ptr<TimerNode>, std::deque<shared_ptr<TimerNode>>, TimerNode_cmp> timer_queue;
public:
    Timer_Manager();
    ~Timer_Manager();
    void add_timer(shared_ptr<Client> client, int timeout);
    void handle_expired_events();
};

#endif
