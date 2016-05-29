#include <iostream>
#include "SocksTunnelLocal.h"
#include "SocksTunnelServer.h"

using namespace std;

int main(int argc, char *argv[])
{
    std::string file;
    if(argc > 1)
        file = argv[1];
    else
    {
        char buf[2048];
        file = std::string(getcwd(buf, sizeof(buf))) + "/setting.json";
    }
    std::cout << "Setting file:" << file << endl;
    serverInit(file);
    SocksTunnelServer tunnel;
    tunnel.start();
    return 0;
}
