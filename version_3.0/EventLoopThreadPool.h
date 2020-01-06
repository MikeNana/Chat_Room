//事件循环线程池
//内含多个事件循环线程，到来的新连接会被分配到不同的线程中，所有连接被平均分配到所有线程中


#ifndef _EventLoopThreadPool_H_
#define _EventLoopThreadPool_H_

#include <memory>
#include <vector>


using std::shared_ptr;
using std::vector;

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
private:
    int num_threads_;
    bool started_;
    bool quit_;
    int loop_pos_;
    vector<EventLoop*> loops_;
    vector<shared_ptr<EventLoopThread>> threads_;

    EventLoop* base_loop_;
public:
    EventLoopThreadPool(int num_threads);
    ~EventLoopThreadPool();
    void start_pool();
    EventLoop* get_next_loop();
    
};

#endif