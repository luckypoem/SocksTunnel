#ifndef __H_SOCK_CLIENT__
#define __H_SOCK_CLIENT__

#include <iostream>
#include "SocksTunnelLocal.h"
#include "SocksTunnelServer.h"
#include "Crypto/CryptoHelper.h"
#include "Crypto/Rc4.h"
#include "Utils/SettingUtils.h"
#include "Crypto/Xor.h"

using namespace std;

int main(int argc, char *argv[])
{
    std::string file = "./setting.json";
    if(argc > 1)
        file = argv[1];
    serverInit(file);
    SocksTunnelLocal tunnel;
    tunnel.start();
    return 0;
}

#endif
