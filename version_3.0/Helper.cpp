#include "Helper.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <sys/unistd.h>
#include <sys/types.h>
#include <string.h>
#include <vector>
#include <fcntl.h>
#include <sys/signal.h>

using std::vector;
using std::string;
using std::cout;
using std::endl;

static const int port = 7999;
static const int MAXFDS = 10000;
static const int MAX_BUFF = 1000;

const int on = 1;
void handle_error(const string& str)
{
    cout << str << endl;
    exit(-1);
}
int socket_and_bind()
{
    int listenfd = 0;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
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

ssize_t read_str(int fd, std::string &inBuffer, bool &zero, int& length) 
{
  ssize_t nread = 0;
  ssize_t readSum = 0;
  while (true) {
    char buff[MAX_BUFF];
    //cout << strlen(buff) << endl;
    if ((nread = read(fd, buff, MAX_BUFF)) < 0) {
      if (errno == EINTR)
        continue;
      else if (errno == EAGAIN) {
        return readSum;
      } else {
        perror("read error");
        return -1;
      }
    } else if (nread == 0) {
      // printf("redsum = %d\n", readSum);
      zero = true;
      break;
    }
//表示没读到任何内容
    length += strlen(buff);
    if(length == 0)
        break;
    // printf("before inBuffer.size() = %d\n", inBuffer.size());
    // printf("nread = %d\n", nread);
    readSum += nread;
    // buff += nread;
    inBuffer += std::string(buff, buff + length);
    // printf("after inBuffer.size() = %d\n", inBuffer.size());
  }
  return readSum;
}

ssize_t write_str(int fd, std::string& sbuff) {
  size_t nleft = sbuff.size();
  ssize_t nwritten = 0;
  ssize_t writeSum = 0;
  const char *ptr = sbuff.c_str();
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0) {
        if (errno == EINTR) {
          nwritten = 0;
          continue;
        } else if (errno == EAGAIN)
          break;
        else
          return -1;
      }
    }
    writeSum += nwritten;
    nleft -= nwritten;
    ptr += nwritten;
  }
  if (writeSum == static_cast<int>(sbuff.size()))
    //sbuff.clear();
    ;
  else
    sbuff = sbuff.substr(writeSum);
  return writeSum;
}

int set_non_block(int fd)
{
  int flag = fcntl(fd, F_GETFL, 0);
  if(flag == -1)
    return -1;
  
  flag |= O_NONBLOCK;
  if(fcntl(fd, F_SETFL, flag) == -1)
    return -1;
  return 0;
}

void handle_for_signal()
{
  struct sigaction sa;
  memset(&sa, '\0', sizeof(sa));
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  if(sigaction(SIGPIPE, &sa, NULL))
    return;
}