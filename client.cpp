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
    if(argc != 3)
    {
        perror("argument error.");
        exit(-1);
    }
    int connfd;
    int message_len = 0;
    int read_len = 0, write_len = 0;
    char message[MAX_MESSAGE] = {0};
    struct sockaddr_in serv_addr;
    socklen_t servaddr_len = sizeof(serv_addr);

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
    
    //开始读写数据
    while(1)
    {
        memset(message, 0, 10); 
        //fputs("请输入数据", stdout);
        fgets(message, MAX_MESSAGE, stdin);
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        write_len = write(connfd, message, MAX_MESSAGE);
        //cout << "write done, begin block." << endl;
        bzero(message, MAX_MESSAGE);
        read_len = read(connfd, message, MAX_MESSAGE);
        printf("echo from server: %s", message);
    }
    close(connfd);
    return 0;
}