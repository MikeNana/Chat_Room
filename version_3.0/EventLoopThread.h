//Header for eventLoopThread
//one loop per thread
#include "EventLoop.h"
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>

using namespace std;

class EventLoopThread
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* start();
private:
    EventLoop* loop_;
    void thread_func();
    std::thread thread_;
    mutex mtx_;
    condition_variable cv_;
};