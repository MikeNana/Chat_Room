#include "EventLoop.h"
#include "Epoll.h"
#include "Helper.h"

#include <iostream>
#include <cassert>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
#include <set>
#include <unistd.h>
using std::unique_lock;
using std::cout;
using std::endl;
using std::set;

extern set<int> link_pool;

EventLoop::EventLoop()
    :
    looping_(false),
    quit_(false),
    poller_(new Epoller()),
    thread_id_(std::this_thread::get_id()),
    mtx_(),
    cv_(),
    pwake_channel_(new Channel(this))
{}

EventLoop::~EventLoop()
{}

void EventLoop::start_loop()
{
    assert(is_in_loop());
    assert(!looping_);
    handle_for_signal();
    vector<shared_ptr<Channel>> ret;
    looping_ = true;
    quit_ = false;
    
    //cout << "enter loop" << endl;

    while(!quit_)
    {
        handle_for_signal();
        cout << get_epollfd_() << endl;
        ret.clear();
        ret = poller_->poll();

        //cout << "断点" << endl;
 
        cout << ret.size() << endl;

        for(auto& it : ret)
            it->handle_event();
        do_pending_functors();
        for(auto a : link_pool)
        {
            cout << a << endl;
            write_str(a, buffer_);
        }
        buffer_.clear();
        poller_->handle_expired_events();
    }

    looping_ = false;
}
void EventLoop::assert_in_loop()
{
    assert(is_in_loop());
}

bool EventLoop::is_in_loop()
{
    return std::this_thread::get_id() == thread_id_;
}

void EventLoop::queue_in_loop(functor&& cb)
{
//由于在放置任务时，有可能主线程和自身线程同时对任务队列进行操作，所以需要加锁，此处可以改进
    {
        unique_lock<mutex> lck;
        pending_functors_.emplace(std::move(cb));
    }
    return;
}

void EventLoop::run_in_loop(functor&& cb)
{
    if(is_in_loop())
        cb();
    else
    {
        queue_in_loop(std::move(cb));
    }
}

void EventLoop::do_pending_functors()
{

//使用交换对象所有权的方法来减少临界区大小
    queue<functor> functors;
    {
        unique_lock<mutex> lck(mtx_);
        functors.swap(pending_functors_);
    }
//执行队列中所有的任务
    while(!functors.empty())
    {
        cout << "doing in" << endl;
        
        functors.front()();
        functors.pop();
    }
}

//当有新的连接到来时，需要在轮询器中添加新的定时器节点
void EventLoop::handle_newconn(shared_ptr<Channel> channel_, int timeout)
{
    poller_->epoll_add(channel_, timeout);
}


void EventLoop::handle_curconn(shared_ptr<Channel> channel_)
{
    {
        unique_lock<mutex> lck(mtx_);
        link_pool.erase(channel_->get_fd());
    }
    close(channel_->get_fd());
}