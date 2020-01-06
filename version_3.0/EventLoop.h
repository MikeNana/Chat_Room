#ifndef _EventLoop_H_
#define _EventLoop_H_

#include "Epoll.h"
#include "Channel.h"
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <string>

using std::string;
using std::function;
using std::queue;
using std::mutex;
using std::condition_variable;
using std::enable_shared_from_this;

class Channel;
class Epoller;

typedef function<void()> functor;

//继承自enable_shared_from_this<EventLoop>,可以在类的定义中使用自己的shared_ptr
class EventLoop : public enable_shared_from_this<EventLoop>
{
private:
    bool looping_;
    bool quit_;
    shared_ptr<Epoller> poller_;
    //Epoller poller_;
//暂定用queue来作为任务队列
    queue<functor> pending_functors_;
    std::thread::id thread_id_;
    mutex mtx_;
    condition_variable cv_;
//用于监听新事件的到来
    shared_ptr<Channel> pwake_channel_;

public:
    EventLoop();
    ~EventLoop();
//void start_loop();该函数应该由EventLoopThread来调用
    void start_loop();
    void assert_in_loop();
    bool is_in_loop();

    void queue_in_loop(functor&& cb);
    void run_in_loop(functor&& cb);

    void do_pending_functors();

    void handle_newconn(shared_ptr<Channel> channel_, int timeout);
    void handle_curconn();
//参数未定，可能是shared_ptr<Channel>
    void grace_shutdown();

    int get_epollfd_(){ poller_->get_epoll_fd_(); }
//暂时放在public
    string buffer_;
};

#endif