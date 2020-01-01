
#include <functional>
#include <memory>

using std::function;
using std::shared_ptr;
using std::weak_ptr;

typedef function<void()> CALLBACK; 
class Channel
{
private:
    int connfd_;
    EventLoop* loop_;
    weak_ptr<Client> holder_;

    CALLBACK&& read_handler_;
    CALLBACK&& write_handler_;
    CALLBACK&& conn_handler_;

public:
    Channel(EventLoop* loop, int connfd);
    ~Channel();
    void set_holder(shared_ptr<Client> holder);
    void set_readhandler(CALLBACK&& read_handler);
    void set_writehandler(CALLBACK&& write_handler);
    void set_connhandler(CALLBACK&& conn_handler);
    
};