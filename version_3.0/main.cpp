#include "Server.h"
#include "EventLoop.h"

using namespace std;

int main()
{
    EventLoop main_loop;
    int threads_num = 4;
    Server server(4, &main_loop);
    server.start();
    main_loop.start_loop();

    return 0;
}