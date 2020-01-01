
#include "Client.h"
#include <memory>
#include <queue>

using std::priority_queue;
using std::shared_ptr;

class TimerNode
{
private:
    int expired_time_;
    shared_ptr<Client> client_;
public:
    TimerNode(shared_ptr<Client> client, int timeout);
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
    void add_timer(shared_ptr<Client> client, int connfd);
    void handle_expired_events();
};