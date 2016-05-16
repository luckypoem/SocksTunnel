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
    SocksTunnelServer tunnel;
    tunnel.start();
    return 0;
}