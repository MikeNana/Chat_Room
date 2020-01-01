#include "Server.h"
#include "Helper.h"
#include <sys/epoll.h>
#include <iostream>
#include <vector>

using namespace std;

Server::Server(int thread_num, EventLoop* server_loop)
    :
    started_(false),
    quit_(false),
    thread_nums_(thread_num),
    server_loop_(server_loop),
    thread_pool_(new EventLoopThreadPool(thread_nums))
{}

Server::~Server()
{}

//主线程启动需要完成的工作包括:
//1.启动线程池
//2.将监听新连接这一事件注册到main_loop_中
void Server::start()
{
    started_ = true;
    thread_pool_.start_pool();
    int listen_fd = socket_and_bind();
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    vector<int> num;
    
}