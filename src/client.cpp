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

int main()
{
    serverInit();
    SocksTunnelLocal tunnel;
    tunnel.start();
    return 0;
}

#endif
