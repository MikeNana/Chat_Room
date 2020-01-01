#include "Channel.h"
#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include <memory>

using std::shared_ptr;

static const int port = 8000;

class EventLoopThreadPool;
class Channel;

class Server
{
private:
    bool started_;
    bool quit_;
    int thread_nums_;
    EventLoopThreadPool thread_pool_;
//server_loop_会管理连接事件的到来
    EventLoop* server_loop_;
public:
    Server(int thread_num, EventLoop* server_loop);
    ~Server();
    void start();
//处理新连接
    void handle_newconn();
};