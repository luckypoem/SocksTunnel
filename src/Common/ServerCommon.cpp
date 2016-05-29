//
// Created by sen on 16-5-14.
//

#include <sys/param.h>
#include "ServerCommon.h"
#include "../Crypto/Chacha20.h"

int createLocalServer(const char *address, uint16_t port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int ret = inet_pton(AF_INET, address, &addr.sin_addr);
    evutil_make_listen_socket_reuseable(fd);
    EXIT_IF(ret != 1, "INET_PTON failed! ret:%d", ret);
    ret = bind(fd, (sockaddr *)&addr, sizeof(addr));
    EXIT_IF(ret != 0, "Bind failed! ret:%d", ret);

    ret = listen(fd, 10);
    EXIT_IF(ret != 0, "Listen failed, ret:%d", ret);
    evutil_make_socket_nonblocking(fd);
    QERROR("Success bind address:%s, port:%d", address, (int)port);
    SettingUtils &setting = SettingUtils::newInstance();
    std::vector<String> ms;
    setting.getMethod(ms);
    QERROR("Password:%s\nCurrent user:%s\nCurrent pwd:%s\nLocal addr:%s port:%d\nRemote addr:%s port:%d\nMethod:%s\nAuth timeout:%d",
           setting.getPassword().c_str(),
           setting.getCurUser().c_str(),
           setting.getCurPwd().c_str(),
           setting.getLocalServer().c_str(),
           setting.getLocalPort(),
           setting.getRemoteServer().c_str(),
           setting.getRemotePort(),
           utils::StringUtils::toString(ms).c_str(),
           setting.getAuthTimeout()
    );
    return fd;
}

//for local client
//ip, port
int createRemoteServer(const char *addr, uint16_t port)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    int ret = inet_pton(AF_INET, addr, &address.sin_addr);
    if(ret != 1)
    {
        QERROR("Inet pton failed for addr:%s", addr);
        return -1;
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(fd);
    connect(fd, (struct sockaddr *)&address, sizeof(address));
    QDEBUG("Connect to remote addr:%s, Fd:%d", addr, fd);
    return fd;
}

//m....is bad...
//for remote client
int createRemoteServer(const char *addr, int addrLen, uint16_t port, char type)
{
    uint32_t host = 0;
    struct sockaddr_storage storage;

    if(type == 0x01)   //IPv4
    {
        struct sockaddr_in *tmp = (struct sockaddr_in *)&storage;
        tmp->sin_family = AF_INET;
        memcpy(&(tmp->sin_addr.s_addr), addr, (size_t)addrLen);
        tmp->sin_port = htons(port);
    }
    else if(type == 0x03)  //Domain
    {
        //TODO:ADD CACHED HERE...
        struct addrinfo *af = NULL, *ptr = NULL;
        int ret = 0;
        char *tmpHost = new char[addrLen + 1];
        memcpy(tmpHost, addr, (size_t)addrLen);
        tmpHost[addrLen] = 0;
        bool isError = false;
        do
        {
            if((ret = getaddrinfo(tmpHost, NULL, NULL, &af)))
            {
                QERROR("Get host by name failed, address:%s, err info:%s", tmpHost, gai_strerror(ret));
                isError = true;
                break;
            }
            ptr = af;
            while(ptr)
            {
                if(ptr->ai_family == AF_INET)
                    break;
                ptr = ptr->ai_next;
                
            }
            if(ptr->ai_family !=  AF_INET && ptr == af)
            {
                QERROR("Couldn't found ipv4 addr, address:%s", tmpHost);
                isError = true;
                break;
            }
            struct sockaddr_in *tmp = (struct sockaddr_in *)&storage;
            tmp->sin_family = AF_INET;
            memcpy(&(tmp->sin_addr.s_addr), &((struct sockaddr_in *)(ptr->ai_addr))->sin_addr, (size_t)ptr->ai_addrlen);
            char ip[17];
            if(!inet_ntop(AF_INET, &((struct sockaddr_in *)(ptr->ai_addr))->sin_addr, ip, sizeof(ip)))
            {
                QERROR("Inet_ntop failed, address:%s, msg:%s", addr, strerror(errno));
            }
            ip[16] = '\0';
            QERROR("Get host by name success, address:%s, ip:%s", addr, ip);
            tmp->sin_port = htons(port);
        }
        while(false);
        if(af)
            freeaddrinfo(af);
        if(tmpHost)
            delete[] tmpHost;
        if(isError)
        {
            return -1;
        }
    }
    else if(type == 0x04)   //IPv6
    {
        struct sockaddr_in6 *tmp = (struct sockaddr_in6 *)&storage;
        tmp->sin6_family = AF_INET6;
        memcpy(&(tmp->sin6_addr.s6_addr16), addr, (size_t)addrLen);
        tmp->sin6_port = htons(port);
    }
    else
    {
        QERROR("Unknow type...:%d,", (int)type);
        return -1;
    }

    int fd = socket(storage.ss_family, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(fd);
    connect(fd, (struct sockaddr *)&storage, sizeof(storage));
    QDEBUG("Send connect request to address:%s, Fd:%d", utils::String(addr, addrLen).c_str(), fd);
    return fd;
}

bool sendAll(int fd, const char *msg, uint32_t len)
{
    ssize_t count = 0, pos = 0;
    bool ret = true;
    int times = 0;
    while(true)
    {
        pos = write(fd, msg + count, (size_t)(len - count));
        if(pos <= 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                QERROR("May send too many data? Fd:%d, len:%d", fd, len);
                ++times;
                continue;
            }
            ret = false;
            break;
        }
        count += pos;
        if(count == len)
            break;

        if(times > 10)
        {
            ret = false;
            break;
        }
    }
    QDEBUG("Fd:%d, Send all success..., len:%d", fd, len);
    return ret;
}

Server::Server(SocksTunnel *tunnel) : tunnel(tunnel)
{
    readIO = new IO(this, tunnel);
    writeIO = new IO(this, tunnel);
}

Server::~Server()
{
    delete readIO;
    delete writeIO;
}

LocalServer::LocalServer(SocksTunnel *tunnel) : Server(tunnel), stage(Init), remote(NULL)
{

}

RemoteServer::RemoteServer(SocksTunnel *tunnel) : Server(tunnel), local(NULL), stage(Connecting)
{

}

IO::IO(Server *server, SocksTunnel *tunnel) : server(server), tunnel(tunnel), startPos(0), total(0)
{
    buf = new char[REAL_SERVER_BUF];
    memset(&timer, 0, sizeof(timer));
}


void removeLocalServer(LocalServer *server)
{
    if(server == NULL)
        return;
    QDEBUG("Close local Fd:%d", server->readIO->asEvIO()->fd);
    ev_timer_stop(server->tunnel->getLoop(), &server->readIO->timer);
    ev_timer_stop(server->tunnel->getLoop(), &server->writeIO->timer);
    close(server->readIO->asEvIO()->fd);
    ev_io_stop(server->tunnel->getLoop(), server->readIO->asEvIO());
    ev_io_stop(server->tunnel->getLoop(), server->writeIO->asEvIO());
    delete server;
}

void removeRemoteServer(RemoteServer *server)
{
    if(server == NULL)
        return;
    ev_timer_stop(server->tunnel->getLoop(), &server->readIO->timer);
    ev_timer_stop(server->tunnel->getLoop(), &server->writeIO->timer);
    QDEBUG("Close remote Fd:%d", server->readIO->asEvIO()->fd);
    close(server->readIO->asEvIO()->fd);
    ev_io_stop(server->tunnel->getLoop(), server->writeIO->asEvIO());
    ev_io_stop(server->tunnel->getLoop(), server->readIO->asEvIO());
    delete server;
}

void closeLocalAndRemoteServer(LocalServer *local, RemoteServer *remote)
{
    removeLocalServer(local);
    removeRemoteServer(remote);
}

void serverInit(const String &file)
{
#ifndef FOREGROUND_START
    daemonize();
#endif

    SettingUtils &setting = SettingUtils::newInstance();
    setting.init(file);
    Logger::newInstance().init(setting.getLogSettingFile());
    std::vector<std::string> methods;
    setting.getMethod(methods);
    for(auto &item : methods)
    {
        QERROR("Begin add method:%s", item.c_str());
        if(item == "xor")
            CryptoHelper::newInstance().pushCrypto(CryptoWrap(new Xor()));
        else if(item == "rc4")
            CryptoHelper::newInstance().pushCrypto(CryptoWrap(new Rc4()));
        else if(item == "chacha20")
            CryptoHelper::newInstance().pushCrypto(CryptoWrap(new Chacha20()));
        else
        {
            QERROR("Not found method:%s, please add it here", item.c_str());
        }
    }
    
}

void daemonize()
{
    int pid;
    pid = fork();
    if(pid < 0)
        exit(1);
    else if(pid > 0)
        exit(0);
    setsid();
    pid = fork();
    if(pid > 0)
        exit(0);
    else if(pid < 0)
        exit(1);
    for(int i=0; i < NOFILE; i++)
        close(i);
    chdir("/");
    umask(0);
    return;
}
