//Header file for eventloop
//1.开始loop
//2.优雅关闭连接而不是简单地close
//3.将任务放在eventloop中，并执行
#include "Timer.h"
#include <functional>
#include <memory>
#include <vector>

using std::vector;
using std::shared_ptr;
using std::function;

class Timer_Manager;
class Channel;

class EventLoop
{

public:
    typedef function<void()> functor;
    typedef shared_ptr<Channel> SP_Channel;
    EventLoop();
    ~EventLoop();
    
    void loop();
    void quit();

    void run_in_loop();
    void queue_in_loop();
    bool is_in_loop();
    void assert_in_loop();

    void shutdown(SP_Channel channel);

    void remove_from_poller(SP_Channel channel);
    void update_poller(SP_Channel channel, int timeout);
    void add_poller(SP_Channel channel, int timeout);

private:     
    bool loop_;
    bool quit_;
    bool event_handling_;
    bool calling_pending_functors_;

    int wakeup_fd_;
    const pid_t thread_id_;

    vector<functor> pending_functors_;
    shared_ptr<Channel> channel_;
    shared_ptr<Epoller> poller_;

    void wakeup();
    void handle_read();
    void handle_conn();
    void do_pending_functors_();
};