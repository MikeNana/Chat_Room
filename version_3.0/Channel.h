//Header file for channel
//function:
//1.管理对应的fd
//2.设置读写控制器供Msgdata初始化时调用
//3.事件分配器

#include "EventLoop.h"
#include <memory>
#include <functional>

class EventLoop;
class Msgdata;

class Channel
{
public:
    typedef std::function<void()> CALLBACK;
    typedef std::shared_ptr<EventLoop> SP_EventLoop;
    Channel(SP_EventLoop loop, int connfd);
    Channel(SP_EventLoop loop);
    ~Channel();
    int get_fd();
    void set_fd();
    void set_holder(shared_ptr<Msgdata> message);
    shared_ptr<Msgdata> get_holder();

    void set_readhandler();
    void set_writehandler();
    void set_connhandler();
    void set_errorhandler();

    void handle_events();
    
private:
    int connfd_;

    void handle_read();
    void handle_write();
    void handle_conn();
    CALLBACK read_handler_;
    CALLBACK write_handler_;
    CALLBACK 
};