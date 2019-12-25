//服务器设计的基本步骤:socket->bind->listen->accept
#include <sys/socket.h>   //connect,send,recv,setsockopt等
#include <sys/types.h>      

#include <netinet/in.h>     // sockaddr_in, "man 7 ip" ,htons
#include <poll.h>             //poll,pollfd
#include <arpa/inet.h>   //inet_addr,inet_aton
#include <unistd.h>        //read,write
#include <netdb.h>         //gethostbyname

#include <error.h>         //perror
#include <stdio.h>
#include <errno.h>         //errno

#include <string.h>          // memset
#include <string>
#include <iostream>

#define port 8000

using namespace std;

static const int MAX_MESSAGE = 10000;
void handle_error(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char** argv)
{
    if(argc != 2){ handle_error(" usage ./Server <port>" ); }

    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){ handle_error(" listen failed.\n" ); }

//初始化服务端地址结构
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//绑定
    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        handle_error("bind error");
    }
//监听
    if(listen(listenfd, 3) < 0)
    {
        handle_error("listen failed");
        close(listenfd);
    }

    cout << "after listen..." << endl;
    
    while(1)
    {
        struct sockaddr_in cli_addr;
        socklen_t cliaddr_len = sizeof(cli_addr);
        int message_len = 0;
        int read_len = 0;
        int connfd;
        char message_recv[MAX_MESSAGE] = {0};
        char message_send[MAX_MESSAGE] = {0};
        char buf[MAX_MESSAGE] = {0};
        cout << "before accept" << endl;
//接受连接请求
        if((connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &cliaddr_len)) < 0)
        {
            perror("accept failed");
            continue;
        }
        else
        {
            printf("client %s connected.\n", inet_ntoa(cli_addr.sin_addr));
        }
        
//接收数据并echo
        while(1)
        {
            if((read_len = read(connfd, buf, MAX_MESSAGE)) < 0)
            {
                perror("read failed\n");
                break;
            }
            printf("message from client %s: %s",inet_ntoa(cli_addr.sin_addr), buf);
            fgets(buf, MAX_MESSAGE, stdin);
            write(connfd, buf, read_len);
            memset(buf, 0, sizeof(buf));
        }
        close(connfd);
    }
    close(listenfd);
    return 0;
}