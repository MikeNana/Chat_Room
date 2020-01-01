
#include "Epoll.h"
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using std::function;
using std::queue;
using std::mutex;
using std::condition_variable;

typedef function<void()> functor;

class EventLoop
{
private:
    Epoller poller_;
//暂定用queue来作为任务队列
    queue<functor> pending_functors_;
    std::thread::id thread_id_;
    mutex mtx;
    condition_variable cv_;

public:
    EventLoop();
    ~EventLoop();
    void start_loop();
    void assert_in_loop();
    bool is_in_loop();

    void queue_in_loop(functor&& cb);
    void run_in_loop(functor&& cb);

    void do_pending_functors();

    void handle_newconn();
    void handle_curconn();
//参数未定，可能是shared_ptr<Channel>
    void grace_shutdown();
};