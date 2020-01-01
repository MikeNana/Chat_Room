#include "Timer.h"
#include "EventLoop.h"
#include "Channel.h"
#include <string>
#include <memory>

using std::shared_ptr;
using std::weak_ptr;
using std::string;

typedef shared_ptr<Channel> SP_Channel;
class Client
{
private:
    int connfd_;
    string in_buffer_;
    string out_buffer_;
    shared_ptr<Channel> channel_;
    EventLoop* loop_;
    TimerNode timer_;
    void handle_read();
    void handle_write();

public:
    Client(EventLoop* loop, int connfd);
    ~Client();
    int get_fd() const();
    SP_Channel get_channel();
    EventLoop* get_loop();

    void link_timer(const TimerNode& tn);
    void seperate_timer();
};