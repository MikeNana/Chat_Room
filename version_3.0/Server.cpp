
#include "Server.h"
#include "Helper.h"
#include "EventLoop.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include <cstring>

using std::cout;
using std::endl;
using std::bind;

extern set<int> link_pool;
class EventLoop;
class EventLoopThreadPool;
class Server;
class Epoller;

Server::Server(int thread_num, EventLoop* server_loop)
    :
    started_(false),
    quit_(false),
    thread_nums_(thread_num),
    listen_fd_(socket_and_bind()),
    thread_pool_(new EventLoopThreadPool(thread_nums_)),
    server_loop_(server_loop),
    accept_channel_(new Channel(server_loop_))
{
    accept_channel_->set_fd(listen_fd_);
}

Server::~Server()
{}

//主线程启动需要完成的工作包括:
//1.启动线程池
//2.将监听新连接这一事件注册到main_loop_中
int Server::start()
{
    started_ = true;
    thread_pool_->start_pool();
    
    accept_channel_->set_readhandler(bind(&Server::handle_newconn, this));
    accept_channel_->set_events(EPOLLIN | EPOLLET);
    accept_channel_->set_revents(0);
    
    server_loop_->handle_newconn(accept_channel_, 0);
    
/*
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listen_fd_;

//创建一个epoll对象来轮询listen_fd
    int epoll_fd_ = server_loop_->get_epollfd_();
//用main_loop中的管理器来管理accept_channel
*/  
    return listen_fd_;
}


void Server::handle_newconn()
{
//处理新连接,将产生的新已连接描述符添加到set中，并将该事件和某个EventLoop连接起来
    struct sockaddr_in cli_addr_;
    memset(&cli_addr_, 0, sizeof(cli_addr_));
    socklen_t cli_addr_len_ = sizeof(cli_addr_);
    int accept_fd_;
    
    while((accept_fd_ = accept(listen_fd_, (struct sockaddr*)&cli_addr_, &cli_addr_len_)) > 0)
    {
//首先取出线程池中的下一个EventLoop
        EventLoop* cur_loop = thread_pool_->get_next_loop();
        link_pool.insert(accept_fd_);
        if(set_non_block(listen_fd_) < 0)
        {
            cout << "set listen_fd failed " << endl;
            return;
        }
        /*
        for(auto a : link_pool)
            cout << a << endl;
        */
        shared_ptr<Client> new_cli(new Client(cur_loop, accept_fd_));
        new_cli->get_channel()->set_holder(new_cli);
        server_loop_->queue_in_loop(bind(&Client::new_conn, new_cli));
        set_non_block(accept_fd_);
        cout << "new connection links eventloop " << new_cli->get_loop()->get_epollfd_() << endl;
    }
//因为是ET模式，所以需要将accept_channel重新置位
    accept_channel_->set_events(EPOLLIN | EPOLLET);
}