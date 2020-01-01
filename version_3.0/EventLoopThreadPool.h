#include "EventLoop.h"
#include "EventLoopThread.h"
#include <vector>

using std::vector;

class EventLoopThreadPool
{
private:
    int num_threads_;
    bool started_;
    bool quit_;
    vector<EventLoop*> loops_;
    vector<EventLoopThread> threads_;
public:
    EventLoopThreadPool(int num_threads);
    ~EventLoopThreadPool();
    void start_pool();
    EventLoop* get_next_loop();
    
};