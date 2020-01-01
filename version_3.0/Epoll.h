
#include "Timer.h"
#include <vector>
#include <memory>
#include <sys/epoll.h>

using std::vector;
static const int MAXFDS = 1000;

class Epoller
{
private:
    int epoll_fd_;
    EventLoop* loop_;
    Timer_Manager timer_manager_;
    shared_ptr<Client> fd_client[MAXFDS];
    shared_ptr<Channel> fd_channel[MAXFDS];

public:
    Epoller(EventLoop* loop);
    ~Epoller();

    void epoll_add(shared_ptr<Client> client_, int timeout);
    void epoll_mod(shared_ptr<Client> client_, int timeout);
    void epoll_del(shared_ptr<Client> client_);
    void add_timer();
    void handle_expired_events();
    vector<epoll_event> poll();
    vector<epoll_event> get_ready_events();
};