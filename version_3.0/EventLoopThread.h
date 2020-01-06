//事件循环线程
//主要是将事件循环进行封装

#ifndef _EventLoopThread_H_
#define _EventLoopThread_H_

#include <mutex>
#include <condition_variable>
#include <thread>

using std::thread;
using std::mutex;
using std::condition_variable;


class EventLoop;

class EventLoopThread
{
private:
    bool quit_;
    bool started_;
    mutex mtx_;
    condition_variable cv_;
    thread thread_;
    EventLoop* loop_;
    void thread_func();
 
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* start_loop();
    
};

#endif