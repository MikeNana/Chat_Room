#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "Channel.h"
#include <memory>

using namespace std;
class EventLoop;

class Server
{
public:
    Server(EventLoop* loop, int threads_num);
    ~Server();
    EventLoop* get_loop();
    void start();
    void handle_newconn();
    void handle_this_conn();
private:
//整个服务器中有且只有一个线程池，所以用unique_ptr
    unique_ptr<EventLoopThreadPool> thread_pool;
    shared_ptr<Channel> server_channel;
    EventLoop* loop_;
    int threads_num;
    bool quit_;
    bool started_;
    int port_;
    int listen_fd_;
    static const int MAXFDS = 10000;
};