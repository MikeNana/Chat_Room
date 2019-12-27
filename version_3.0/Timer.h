//Header file for timernode and timer_manager
//timernode function:
//1.判断自身是否超时，是否应该删除
//2.获取与自己相关的一些信息

//timer_manager function
//1.处理超时事件
//2.添加时间节点
#include <queue>
#include <vector>
#include <memory>

using std::shared_ptr;
using std::priority_queue;
using std::vector;

class Msgdata;

class TimerNode
{
public:
    typedef shared_ptr<Msgdata> SP_Msgdata;
    TimerNode(SP_Msgdata message, int timeout);
    ~TimerNode();
    TimerNode(const TimerNode& tn);
    void update(int timeout);
    bool isValid();
    void clear_request();
    void set_deleted();
    bool is_deleted();
    size_t get_exp_time();
private:
    bool deleted_;
    size_t expired_time;
    SP_Msgdata message;
};


struct timernode_cmp
{
    bool operator()(shared_ptr<TimerNode>& tn1, shared_ptr<TimerNode>& tn2)
    {
        return tn1->get_exp_time() < tn2->get_exp_time();
    }
};
class Timer_Manager
{
public:
    Timer_Manager();
    ~Timer_Manager();
    void add_timernode(shared_ptr<Msgdata> message, int timeout);
    void handle_expried_node();
private:
    typedef shared_ptr<TimerNode> SP_Timernode;
    priority_queue<SP_Timernode, std::deque<SP_Timernode>, timernode_cmp> timer_node_queue;
};