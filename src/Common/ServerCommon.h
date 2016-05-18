//
// Created by sen on 16-5-14.
//

#ifndef SOCKSTUNNELLOCAL_SERVERCOMMON_H
#define SOCKSTUNNELLOCAL_SERVERCOMMON_H

#include <ev.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <event2/util.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../CommonLib/Logger.h"
#include "../CommonLib/NoCopyable.h"
#include "../Crypto/CryptoHelper.h"
#include "../Crypto/Xor.h"
#include "../Crypto/Rc4.h"
#include "../Utils/SettingUtils.h"
#include <vector>
#include <netdb.h>

#ifndef MAX_SERVER_BUF
#define MAX_SERVER_BUF (4096)
#endif

#ifndef REAL_SERVER_BUF
#define REAL_SERVER_BUF (MAX_SERVER_BUF + 1024)
#endif

struct IO;
struct LocalServer;
struct RemoteServer;

using namespace loggable::debugger;

class SocksTunnel : public utils::NoCopyable
{
public:
    SocksTunnel() { loop_ = EV_DEFAULT; }
    virtual ~SocksTunnel() { ev_loop_destroy(loop_); }

public:
    virtual void start() = 0;
    struct ev_loop *getLoop() { return loop_; }

private:
    struct ev_loop *loop_;
};

struct Server : public utils::NoCopyable
{
public:
    Server(SocksTunnel *tunnel);
    ~Server();
    IO *readIO;
    IO *writeIO;
    SocksTunnel *tunnel;
};

struct LocalServer : public Server
{
public:
    LocalServer(SocksTunnel *tunnel);
    enum Stage {Init, HandShake, Connect};
    Stage stage;
    RemoteServer *remote;

};

struct RemoteServer : public Server
{
    RemoteServer(SocksTunnel *tunnel);
    enum Stage { Connecting, Connected };
    Stage stage;
    LocalServer *local;
};

struct IO : public utils::NoCopyable
{
public:
    IO(Server *server, SocksTunnel *tunnel);
    ~IO() { delete[] buf; }
    ev_io io;
    ev_timer timer;
    SocksTunnel *tunnel;
    Server *server;
    char *buf;
    uint32_t startPos;
    uint32_t total;

public:
    void copyBuffer(const char *str, size_t len)
    {
        memmove(buf, str, len);
        startPos = 0;
        total = static_cast<uint32_t>(len);
    }
    void appendBuffer(const char *str, size_t len)
    {
        memmove(buf + total, str, len);
        total += len;
    }

    void appendInt(int val)
    {
        val = htonl(val);
        appendBuffer((char *)&val, 4);
    }

    void compack()
    {
        if(startPos != 0)
        {
            //dont use strncpy or memcpy.what the fk....
            memmove(buf, buf + startPos, total - startPos);
            total -= startPos;
            startPos = 0;
            QDEBUG("After compack, current total:%d", (int)total);
        }
    }
    void swapBuffer(IO *io)
    {
        std::swap(buf, io->buf);
        std::swap(startPos, io->startPos);
        std::swap(total, io->total);
    }

    void resetBuffer()
    {
        //Unnecessary..
        //memset(buf, 0, total);
        startPos = total = 0;
    }
    void resetReadIndex() { startPos = 0; }
    ev_io *asEvIO() { return &io; }
};

int createLocalServer(const char *addr, uint16_t port);
int createRemoteServer(const char *addr, uint16_t port);
int createRemoteServer(const char *addr, int addrLen, uint16_t port, char type);
bool sendAll(int fd, const char *msg, uint32_t len);

void removeLocalServer(LocalServer *server);
void removeRemoteServer(RemoteServer *server);
void closeLocalAndRemoteServer(LocalServer *local, RemoteServer *remote);

void serverInit(const String &file);

#endif //SOCKSTUNNELLOCAL_SERVERCOMMON_H
