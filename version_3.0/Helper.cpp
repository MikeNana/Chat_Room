#include "Helper.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <sys/unistd.h>
#include <sys/types.h>
#include <string.h>

using std::string;
using std::cout;
using std::endl;

static const int port = 8000;
static const int MAXFDS = 10000;
const int on = 1;
void handle_error(const string& str)
{
    cout << str << endl;
    exit(-1);
}
int socket_and_bind()
{
    int listenfd = 0;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    handle_error("socket failed");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        handle_error("bind error");
    if(listen(listenfd, MAXFDS) < 0)
        handle_error("listen error");
//设置可重用标志位
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    return listenfd;
}