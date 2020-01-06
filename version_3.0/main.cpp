#include "Server.h"
#include "EventLoop.h"

using namespace std;
//尝试使用全局变量记录所有的连接，便于传递信息
set<int> link_pool;

int main()
{
    EventLoop main_loop;
    int threads_num = 4;
    Server server(4, &main_loop);
    server.start();
    main_loop.start_loop();
    
    return 0;
}