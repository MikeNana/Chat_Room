#include "EventLoopThread.h"
#include "EventLoop.h"
#include <iostream>
#include <thread>
#include <cassert>
#include <functional>

using std::cout;
using std::endl;
using std::unique_lock;
using std::bind;

EventLoopThread::EventLoopThread()
    :
    mtx_(),
    cv_(),
    quit_(false),
    started_(false), 
    thread_(bind(&EventLoopThread::thread_func, this), "EventLoopThread")
{
    
}   

EventLoopThread::~EventLoopThread()
{
    quit_ = true;
    cout << "current thread exit." << endl;
}

EventLoop* EventLoopThread::start_loop()
{
    assert(!started_);
    thread_.detach();
    {
        unique_lock<mutex> lck(mtx_);
        while(loop_ == nullptr)
            cv_.wait(lck);
    }
    started_ = true;
    //cout << "event loop thread " << std::this_thread::get_id() << " start..." << endl;
    return loop_;
}
//定义线程入口函数
void EventLoopThread::thread_func()
{
    EventLoop loop;
    {
        unique_lock<mutex> lck(mtx_);
        loop_ = &loop;
        cv_.notify_all();
    }
    loop_->start_loop();
    return;
}
