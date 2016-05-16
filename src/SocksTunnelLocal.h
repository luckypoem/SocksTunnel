//
// Created by sen on 16-5-13.
//

#ifndef SOCKSTUNNEL_SOCKSTUNNEL_H
#define SOCKSTUNNEL_SOCKSTUNNEL_H

#include <ev.h>
#include <evutil.h>
#include "CommonLib/Logger.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <algorithm>
#include "CommonLib/NoCopyable.h"
#include "Common/MsgResolver.h"
#include "Common/ServerCommon.h"

class SocksTunnelLocal;

#ifndef MAX_SERVER_BUF
#define MAX_SERVER_BUF 4096
#endif

using namespace loggable::debugger;

class SocksTunnelLocal : public SocksTunnel
{
public:
    SocksTunnelLocal();
    ~SocksTunnelLocal();

public:
    void start();
private:
    static void readCallback(struct ev_loop *loop, ev_io *args, int revents);
    static void writeCallback(struct ev_loop *loop, ev_io *args, int revents);
    static void remoteReadCallback(struct ev_loop *loop, ev_io *args, int revents);
    static void remoteWriteCallback(struct ev_loop *loop, ev_io *args, int revents);
    static void acceptCallback(struct ev_loop *loop, ev_io *args, int revents);
    static void remoteTimeoutCallback(struct ev_loop *loop, ev_timer *timer, int revents);
};


#endif //SOCKSTUNNEL_SOCKSTUNNEL_H
