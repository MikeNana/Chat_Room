//Header file for EventLoopThreadPool
//1.启动线程池
//2.获取下一个eventloop

#include "EventLoopThread.h"
#include <memory>
#include <vector>
using namespace std;

class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop* base_loop_, int thread_num);
    ~EventLoopThreadPool();
    void start_pool();
    EventLoop* get_next_eventLoop();
private:
    EventLoop* base_loop;
    bool started_;
    int num_threads;
    int next_;
    vector<EventLoopThread> eventloopthread_pool;
    vector<EventLoop*> loops_;
};