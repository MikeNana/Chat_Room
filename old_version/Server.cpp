//服务器设计的基本步骤:socket->bind->listen->accept

//版本二:实现多个用户连接服务器端
//服务器需要完成的主要功能接收新的连接和消息转发
#include <sys/socket.h>   //connect,send,recv,setsockopt等
#include <sys/types.h>      
#include <sys/epoll.h>

#include <netinet/in.h>     // sockaddr_in, "man 7 ip" ,htons
#include <poll.h>             //poll,pollfd
#include <arpa/inet.h>   //inet_addr,inet_aton
#include <unistd.h>        //read,write
#include <netdb.h>         //gethostbyname

#include <error.h>         //perror
#include <stdio.h>
#include <errno.h>         //errno
#include <signal.h>         //sigaction
#include <string.h>          // memset
#include <string>
#include <iostream>

#include <vector>
#include <string>
#include <set>
#define port 8000
#define LISTENQ 5
#define MAX_EVENTS 20
#define MAX_MESSAGE 10000

using namespace std;
//打开SO_REUSEADDR设置，避免服务器重启时端口还未释放
static const int on = 1;
void error_handler(const char* str)
{
    printf(" %s ", str);
    exit(1);
}

//对SIGPIPE设置处理函数
void handle_for_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL))
        return ;
}


int main(int argc, char* argv[])
{
    int listen_fd, conn_fd, epoll_fd;
    struct sockaddr_in serv_addr, cli_addr;
    struct epoll_event events[MAX_EVENTS], ev;
    socklen_t cliaddr_len;

    int nfds = 0;
    int read_len = 0;
    char buf[MAX_MESSAGE] = {0};
    char message_recv[MAX_MESSAGE] = {0};
    set<int> fd_set;
//创建socket
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error_handler("socket failed.\n");
//将listenfd设置为reuse，避免重启时端口被占用未被释放
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
//固定服务器地址
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
//绑定监听端口
    if(bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error_handler("bind failed.\n");
//启动监听
    if(listen(listen_fd, LISTENQ) < 0)
        error_handler("listen failed.\n");
//创建epoll_fd并将listen事件注册到epoll中
    epoll_fd = epoll_create(MAX_EVENTS);
    ev.data.fd = listen_fd;
    ev.events = EPOLLIN | EPOLLHUP;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

    handle_for_sigpipe();

//开始epoll轮询，并分类处理各类事件
    while(true)
    {
//暂时将事件返回时间设置为0
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 0);
        for(int i = 0; i < nfds; ++i)
        {
//如果是新事件的到来，则需要接受connect请求，存储该连接的connfd
            if(events[i].data.fd == listen_fd)
            {
                int cur_connfd;
                if((cur_connfd = accept(listen_fd, (struct sockaddr*)&cli_addr, &cliaddr_len)) < 0)
                    error_handler("accept failed.\n");
                cout << "accept a new connection from " << inet_ntoa(cli_addr.sin_addr) << endl;
                ev.data.fd = cur_connfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cur_connfd, &ev);
                fd_set.insert(cur_connfd);
            }
//如果是某连接可读
            else if (events[i].events & EPOLLIN)
            {
                int cur_connfd = events[i].data.fd;
                const char* cli_ip = inet_ntoa(cli_addr.sin_addr);
                strcat(buf, cli_ip);
                strcat(buf, ": ");
                if((read_len = read(cur_connfd, message_recv, MAX_MESSAGE)) < 0)
                    error_handler("read failed.\n");
                strcat(buf, message_recv);
                printf("%s", buf);
//并将该信息转发给其他所有的连接
                for(auto a:fd_set)
                {
                    if(write(a, buf, MAX_MESSAGE) < 0)
                    {
                        cout << "write error" << endl;
                        fd_set.erase(a);
                    }
                    else
                    {
                        cout << "write success" << endl;
                    }
                    
                }
                memset(buf, '\0', sizeof(buf));
                printf("%s", buf);
            }
        }
    }
    if(!fd_set.empty())
    {
        for(auto a : fd_set)
            close(a);
    }
    close(listen_fd);
    return 0;
}