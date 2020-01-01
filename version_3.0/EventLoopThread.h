#include "EventLoop.h"

using namespace std;

class EventLoopThread
{
private:
    EventLoop* loop_;
    void thread_func();
    bool quit_;
    bool started_;

public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* get_loop();
    
};