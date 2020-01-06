#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

#include <assert.h>
#include <iostream>

using std::cout;
using std::endl;

EventLoopThreadPool::EventLoopThreadPool(int num_threads)
    :
    num_threads_(num_threads),
    started_(false),
    quit_(false),
    loop_pos_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{
    quit_ = true;
    cout << "EventLoopThreadPool exited" << endl;
}

void EventLoopThreadPool::start_pool()
{
    started_ = true;
    for(unsigned short i = 0; i < num_threads_; ++i)
    {
        shared_ptr<EventLoopThread> t(new EventLoopThread());
        if(t)
        {
            threads_.emplace_back(t);
            loops_.emplace_back(t->start_loop());
        }
    }
}

EventLoop* EventLoopThreadPool::get_next_loop()
{
    assert(started_);
    EventLoop* ret = base_loop_;
    if(!loops_.empty())
    {
        ret = loops_[loop_pos_];
        loop_pos_ = (loop_pos_ + 1) % num_threads_;
    }
    return ret;
}