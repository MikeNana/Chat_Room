//客户端也要用epoll，否则无法快速读取其他用户发送的信息
#include <sys/socket.h>   //connect,send,recv,setsockopt等
#include <sys/types.h>      

#include <netinet/in.h>     // sockaddr_in, "man 7 ip" ,htons
#include <poll.h>             //poll,pollfd
#include <sys/epoll.h>
#include <arpa/inet.h>   //inet_addr,inet_aton
#include <unistd.h>        //read,write
#include <netdb.h>         //gethostbyname
#include <fcntl.h>
#include <error.h>         //perror
#include <stdio.h>
#include <errno.h>         //errno

#include <string.h>          // memset
#include <string>
#include <iostream>

#define port 8000
#define MAX_EVENTS 10
using namespace std;

static const int MAX_MESSAGE = 10000;
//错误处理函数
void handle_error(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

//设置非阻塞模式函数
int set_nonblock(int fd)
{
    int flags;
    flags = fcntl(fd, F_GETFL);
    if(flags < 0)
        return flags;
    flags |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) < 0)
        return -1;
    return 0;
}

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        perror("argument error.");
        exit(-1);
    }
    int connfd, epoll_fd;
    int message_len = 0;
    int read_len = 0, write_len = 0;
    int nfds = 0;
    char message_send[MAX_MESSAGE] = {0};
    char message_recv[MAX_MESSAGE] = {0};
    struct sockaddr_in serv_addr;
    socklen_t servaddr_len = sizeof(serv_addr);
    struct epoll_event ev, events[MAX_EVENTS];
    if((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        handle_error("socket error\n");
    }
    
    bzero(&serv_addr, servaddr_len);
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(connfd, (struct sockaddr*)&serv_addr, servaddr_len) < 0)
    {
        handle_error("connect error\n");
    }
    else
    {
        cout << "connect succeed" << endl;
    }
    
    //set_nonblock(connfd); 

//将stdin和服务端连接都放到epoll轮询中，尝试实现同时读写
    epoll_fd = epoll_create(MAX_EVENTS);
    cout << epoll_fd << endl;
    ev.data.fd = connfd;
    ev.events = EPOLLIN | EPOLLHUP | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connfd, &ev);

    ev.data.fd = 0;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &ev);

    while(true)
    {
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 0);
        if(nfds < 0)
            handle_error("epoll failed.\n");
        for(int i = 0; i < nfds; ++i)
        {
//表明标准输入有数据到来
            if(events[i].data.fd == 0)
            {
                //cout << "stdin" << endl;
                read(0, message_send, MAX_MESSAGE);
                write(connfd, message_send, sizeof(message_send));
                memset(message_send, 0, sizeof(message_send));
            }
            else if(events[i].data.fd == connfd)
            {
                //cout << "from server" << endl;
                if(events[i].events & EPOLLIN)
                    read(connfd, message_recv, MAX_MESSAGE);
                printf("%s", message_recv);
                memset(message_recv, 0, sizeof(message_recv));
            }
            else  
                handle_error("file descriptor error.\n");
        }
    }








/*
//将客户端的读功能用epoll来管理，实现实时读取其他用户信息的功能
    ev.data.fd = connfd;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connfd, &ev);
    setvbuf(stdin, NULL, _IONBF, 0);
//开始读写数据
    while(1)
    {
        memset(message_send, 0, sizeof(message_send)); 
        fgets(message_send, MAX_MESSAGE, stdin);
        if(!strcmp(message_send, "q\n") || !strcmp(message_send, "Q\n"))
            break;
        write_len = write(connfd, message_send, MAX_MESSAGE);
        printf("me: %s", message_send);

        if((read_len = read(connfd, message_recv, MAX_MESSAGE)) < 0)
        {
            if(errno == EAGAIN)
                continue;
            perror("read failed.\n");
            exit(1);
        }
        printf("receive from server: %s", message_recv);
        memset(message_recv, 0, sizeof(message_send));
        /*
        nfds = epoll_wait(epoll_fd, events, 10, 0);

        cout << nfds << endl;
        
        bzero(message_recv, MAX_MESSAGE);
        printf("receive from server: %s", message_recv);
        for(int i = 0; i < nfds; ++i)
        {
            if(events[i].events & EPOLLIN)
            {
                if((read_len = read(connfd, message_recv, MAX_MESSAGE)) < 0)
                {
                    perror("read failed.\n");
                    exit(1);
                }
                printf("receive from server: %s", message_recv);
            }
        }
    }
    */
    close(connfd);
    close(epoll_fd);
    return 0;
}