//
// Created by sen on 16-5-14.
//

#include "SocksTunnelServer.h"
#include "Utils/RandomUtils.h"

SocksTunnelServer::SocksTunnelServer()
{

}

SocksTunnelServer::~SocksTunnelServer()
{

}

void SocksTunnelServer::start()
{
    //TODO:use params here...
    int fd = createLocalServer(SettingUtils::newInstance().getRemoteServer().c_str(), SettingUtils::newInstance().getRemotePort());
    LocalServer *server = new LocalServer(this);
    ev_io_init(server->readIO->asEvIO(), acceptCallback, fd, EV_READ);
    ev_io_start(getLoop(), server->readIO->asEvIO());
    ev_run(getLoop(), 0);
}

void SocksTunnelServer::readCallback(struct ev_loop *loop, ev_io *args, int revents)
{
    LocalServer *server = static_cast<LocalServer *>(reinterpret_cast<IO *>(args)->server);
    QDEBUG_IF(server == NULL, "Can't cast to local server?");
    bool isError = false;

    ssize_t count = read(server->readIO->asEvIO()->fd, server->readIO->buf + server->readIO->total, MAX_SERVER_BUF - server->readIO->total);

    if(count == 0 && server->readIO->total == 0)
    {
        isError = true;
        QDEBUG("Server close connect...Fd:%d", server->readIO->asEvIO()->fd);
        closeLocalAndRemoteServer(server, server->remote);
        return;
    }

    if(count < 0)
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        QERROR("Socket error... Fd:%d", server->readIO->asEvIO()->fd);
        isError = true;
        closeLocalAndRemoteServer(server, server->remote);
        return;
    }
    server->readIO->total += count;
    switch(server->stage)
    {
        case LocalServer::Init: //auth and connect...
        {
            if(server->readIO->total < 4)
                return;
            //need more data...
            int total = ntohl(*reinterpret_cast<int *>(server->readIO->buf + server->readIO->startPos));
            if(total < 0)
            {
                isError = true;
                QERROR("I receive a negative number...mmm..it is not excepted... close it.. Fd:%d", server->readIO->asEvIO()->fd);
                break;
            }
            if(total > MAX_SERVER_BUF)
            {
                isError = true;
                QERROR("Msg too long...Fd:%d, total:%d", server->readIO->asEvIO()->fd, total);
                break;
            }
            //need more data...
            if(server->readIO->total < 4 + total)
                return;
            //====DECRYPT====
            String tmp(server->readIO->buf, 4);
            String dataStr(server->readIO->buf + 4, total);
            CryptoHelper::newInstance().decrypt(dataStr, SettingUtils::newInstance().getPassword());
            tmp += dataStr;
            //String decode(server->readIO->buf, total + 4);
            server->readIO->startPos += total + 4;
            AuthMsg msg;
            if(!msg.decode(tmp))
            {
                isError = true;
                QERROR("Msg decode failed, Fd:%d", server->readIO->asEvIO()->fd);
                break;
            }
            QDEBUG("Msg info:%s", msg.info().c_str());

            if(!msg.check())
            {
                isError = true;
                QERROR("Msg check failed, Fd:%d", server->readIO->asEvIO()->fd);
                break;
            }
            if(!SettingUtils::newInstance().checkUser(msg.user, msg.pwd))
            {
                isError = true;
                QERROR("User or password error, fd:%d", server->readIO->asEvIO()->fd);
                break;
            }
            int fd = createRemoteServer(msg.addr.c_str(), (unsigned char)msg.addrLen, msg.port, msg.addrType);
            if(fd < 0)
            {
                isError = true;
                QDEBUG("Got a negative fd...local Fd is:%d", server->readIO->asEvIO()->fd);
                break;
            }
            RemoteServer *remote = (server->remote = new RemoteServer(server->tunnel));
            remote->local = server;
            ev_io_init(remote->readIO->asEvIO(), remoteReadCallback, fd, EV_READ);
            ev_io_init(remote->writeIO->asEvIO(), remoteWriteCallback, fd, EV_WRITE);
            ev_timer_init(&remote->writeIO->timer, remoteTimeoutCallback, 60, 0);
            ev_io_start(remote->tunnel->getLoop(), remote->writeIO->asEvIO());
            ev_timer_start(remote->tunnel->getLoop(), &remote->writeIO->timer);
            server->stage = LocalServer::Connect;
            //ev_io_stop(server->tunnel->getLoop(), server->readIO->asEvIO());
            QDEBUG_IF(server->readIO->startPos > server->readIO->total, "Why startPos > total? Fd:%d", server->readIO->asEvIO()->fd);
            if(server->readIO->startPos < server->readIO->total)
            {
                QDEBUG("Local read buffer spare %d byte after auth, compack it...Fd:%d",
                       server->readIO->total - server->readIO->startPos, server->readIO->asEvIO()->fd
                );
                server->readIO->compack();
            }
            else
                server->readIO->resetBuffer();
            QDEBUG("Begin pass through...Fd:%d", server->readIO->asEvIO()->fd);
            //break;
            //pass through...
        }
        case LocalServer::Connect: //send and recv
        {
            QDEBUG("Fd:%d, In connect...", server->readIO->asEvIO()->fd);
            if(server->remote->writeIO->total != server->remote->writeIO->startPos)
            {
                QERROR("May not connect to remote? or may have not send finish, remote fd:%d", server->remote->writeIO->asEvIO()->fd);
                return;
            }
            if(server->readIO->total < server->readIO->startPos + 4)
                return;
            int msgCount = 0;
            while(true)
            {
                if(server->readIO->startPos + 4 > server->readIO->total)
                    break;
                int total = ntohl(*reinterpret_cast<int *>(server->readIO->buf + server->readIO->startPos));
                QDEBUG("Local recv total:%d", total);
                if (total <= 0)
                {
                    isError = true;
                    QERROR("I receive a negative number...mmm..it is not excepted... close it.. Fd:%d",
                           server->readIO->asEvIO()->fd);
                    break;
                }
                if (total > REAL_SERVER_BUF)
                {
                    isError = true;
                    QERROR("Msg too long...Fd:%d, total:%d", server->readIO->asEvIO()->fd, total);
                    break;
                }
                //need more data;
                if (server->readIO->total < server->readIO->startPos + 4 + total)
                {
                    if(msgCount != 0)
                        break;
                    else
                        return;
                }
                DataMsg msg;
                //====DECRYPT====
                //String tmp(server->readIO->buf + server->readIO->startPos, 4 + total);
                String tmp(server->readIO->buf + server->readIO->startPos, 4);
                String dataStr(server->readIO->buf + server->readIO->startPos + 4, total);
                CryptoHelper::newInstance().decrypt(dataStr, SettingUtils::newInstance().getPassword());
                tmp += dataStr;
                if(!msg.decode(tmp))
                {
                    QERROR("Deocde failed, Fd:%d, Msg size:%d", server->readIO->asEvIO()->fd, (int)tmp.size());
                    isError = true;
                    break;
                }
                QDEBUG("Msg info:%s", msg.info().c_str());
                if(!msg.check())
                {
                    isError = true;
                    QERROR("Msg check failed...Msg size:%d, Fd:%d", (int)tmp.size(), server->readIO->asEvIO()->fd);
                    break;
                }
                server->remote->writeIO->appendBuffer(msg.data.c_str(), msg.data.size());
                server->readIO->startPos += 4 + total;
                ++msgCount;
            }
            if(isError)
                break;
            server->readIO->compack();
            ev_io_stop(server->tunnel->getLoop(), server->readIO->asEvIO());
            ev_io_start(server->tunnel->getLoop(), server->remote->writeIO->asEvIO());
            QDEBUG("Fd:%d stop read and change to write Fd:%d, readCount:%d", server->readIO->asEvIO()->fd, server->remote->writeIO->asEvIO()->fd, server->writeIO->total);
            break;
        }
        default:
            isError = true;
            QERROR("Unknow stage:%d, Fd:%d", static_cast<int>(server->stage), server->readIO->asEvIO()->fd);
            break;
    }

    if(isError)
    {
        closeLocalAndRemoteServer(server, server->remote);
    }
}

void SocksTunnelServer::writeCallback(struct ev_loop *loop, ev_io *args, int revents)
{
    LocalServer *server = static_cast<LocalServer *>(reinterpret_cast<IO *>(args)->server);
    bool isError = false;
    EXIT_IF(server == NULL, "Can't cast to local server?");
    do
    {
        ssize_t count = write(server->writeIO->asEvIO()->fd, server->writeIO->buf + server->writeIO->startPos, server->writeIO->total - server->writeIO->startPos);
        if(count <= 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
                return;
            QERROR("Write error....");
            isError = true;
            break;
        }
        QDEBUG("Local write...fd:%d, write count:%d, total count:%d", server->writeIO->asEvIO()->fd, (int)count, server->writeIO->total );
        server->writeIO->startPos += count;
        if(server->writeIO->startPos == server->writeIO->total)
        {
            QDEBUG("Local write fd:%d success, count:%d", server->writeIO->asEvIO()->fd, server->writeIO->total);
            server->writeIO->resetBuffer();
            ev_io_stop(server->tunnel->getLoop(), server->writeIO->asEvIO());
            ev_io_start(server->tunnel->getLoop(), server->remote->readIO->asEvIO());
        }
    }
    while(false);

    if(isError)
    {
        closeLocalAndRemoteServer(server, server->remote);
    }
}

void SocksTunnelServer::remoteReadCallback(struct ev_loop *loop, ev_io *args, int revents)
{
    QDEBUG("Remote read....");
    RemoteServer *server = reinterpret_cast<RemoteServer *>(reinterpret_cast<IO *>(args)->server);
    bool isError = false;
    EXIT_IF(server->local == NULL, "Couldn't found local server???");
    do
    {
        ssize_t count = read(server->readIO->asEvIO()->fd, server->readIO->buf + server->readIO->total, MAX_SERVER_BUF - server->readIO->total);
        if(count == 0)
        {
            isError = true;
            QDEBUG("Remote server close connect, Fd:%d", server->readIO->asEvIO()->fd);
            break;
        }
        if(count < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
                return;
            QERROR("Remote read cause error, Fd:%d", server->readIO->asEvIO()->fd);
            isError = true;
            break;
        }
        server->readIO->total += count;
        QDEBUG("Fd:%d, Remote read total:%d, current read index:%d", server->readIO->asEvIO()->fd, server->readIO->total, server->readIO->startPos);

        int sz = 0;
        do
        {
#ifndef NDEBUG
            int avaCount = 500; //RandomUtils::randInt(300, 800);
#else
            int avaCount = RandomUtils::randInt(300, 800);
#endif
            if(avaCount + sz > server->readIO->total)
                avaCount = server->readIO->total - sz;
            QDEBUG("avaCount:%d, sz:%d, total:%d", avaCount, sz, server->readIO->total);
            DataMsg msg;
            String tmp;
            msg.data.assign(server->readIO->buf + sz, avaCount);
            if(!msg.encode(tmp))
            {
                isError = true;
                QERROR("Msg encode failed... Fd:%d", server->readIO->asEvIO()->fd);
                break;
            }
            //===ENCRYPT===
            CryptoHelper::newInstance().encrypt(tmp, SettingUtils::newInstance().getPassword());
            QDEBUG("Append %d byte to local write...Fd:%d", (int)tmp.size(), server->readIO->asEvIO()->fd);
            server->local->writeIO->appendInt((int)tmp.size());
            server->local->writeIO->appendBuffer(tmp.c_str(), tmp.size());
            sz += avaCount;
        }
        while(sz < server->readIO->total);

        if(isError)
            break;
        server->readIO->resetBuffer();
        ev_io_stop(server->tunnel->getLoop(), server->readIO->asEvIO());
        ev_io_start(server->tunnel->getLoop(), server->local->writeIO->asEvIO());
        return;
    }
    while(false);

    if(isError)
    {
        closeLocalAndRemoteServer(server->local, server);
    }
}

void SocksTunnelServer::remoteWriteCallback(struct ev_loop *loop, ev_io *args, int revents)
{
    RemoteServer *server = reinterpret_cast<RemoteServer *>(reinterpret_cast<IO *>(args)->server);
    EXIT_IF(server == NULL, "Could not cast to remote server!!!");
    EXIT_IF(server->local == NULL, "I couldn't found local server!!!!!");
    bool isError = false;
    if(server->stage == RemoteServer::Connecting)
    {
        struct sockaddr_storage addr;
        socklen_t len = sizeof(sockaddr_storage);
        int ret = getpeername(server->writeIO->asEvIO()->fd, (sockaddr *)&addr, &len);
        if(ret == 0)
        {
            QDEBUG("Connect to remote success..Fd:%d", server->writeIO->asEvIO()->fd);
            server->stage = RemoteServer::Connected;
            ev_timer_stop(server->tunnel->getLoop(), &server->writeIO->timer);
            //begin to read data from remote when connected
            ev_io_start(server->tunnel->getLoop(), server->readIO->asEvIO());
        }
        else
        {
            QERROR("Connect to remote failed, Fd:%d", server->readIO->asEvIO()->fd);
            closeLocalAndRemoteServer(server->local, server);
            return;
        }
    }


    if(server->writeIO->total > 0)
    {
        do
        {
            ssize_t count = write(server->writeIO->asEvIO()->fd, server->writeIO->buf + server->writeIO->startPos, server->writeIO->total - server->writeIO->startPos);
            if(count <= 0)
            {
                if(errno == EAGAIN || errno == EWOULDBLOCK)
                    return;
                QERROR("Remote write error...Fd:%d", server->writeIO->asEvIO()->fd);
                isError = true;
                break;
            }
            QDEBUG("Remote write count:%d, Fd:%d", (int)count, server->writeIO->asEvIO()->fd);
            server->writeIO->startPos += count;
        }
        while(false);
    }

    if(server->writeIO->startPos == server->writeIO->total)
    {
        server->writeIO->resetBuffer();
        QDEBUG("Stop remote write and turn to local read, remote Fd:%d, local Fd:%d", server->writeIO->asEvIO()->fd, server->local->readIO->asEvIO()->fd);
        ev_io_stop(server->tunnel->getLoop(), server->writeIO->asEvIO());
        ev_io_start(server->tunnel->getLoop(), server->local->readIO->asEvIO());
    }

    if(isError)
    {
        closeLocalAndRemoteServer(server->local, server);
    }



}

void SocksTunnelServer::acceptCallback(struct ev_loop *loop, ev_io *args, int revents)
{
    Server *server = reinterpret_cast<IO *>(args)->server;
    sockaddr_in client;
    socklen_t clientlen = sizeof(client);
    int fd = accept(args->fd, (sockaddr *)&client, &clientlen);
    if(fd < 0)
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        QERROR("Accept got -1, begin to exit!");
        ev_io_stop(server->tunnel->getLoop(), args);
        close(args->fd);
        delete server;
        exit(0);
    }
    char tmp[17] = {0};
    inet_ntop(AF_INET, &client.sin_addr, tmp, sizeof(tmp));
    QERROR("Client:%s, port:%d connect! Fd:%d", tmp, ntohs(client.sin_port), fd);
    evutil_make_socket_nonblocking(fd);
    Server *clientArgs = new LocalServer(server->tunnel);
    ev_io_init(clientArgs->readIO->asEvIO(), readCallback, fd, EV_READ);
    ev_io_init(clientArgs->writeIO->asEvIO(), writeCallback, fd, EV_WRITE);
    QDEBUG("Begin to listen Fd:%d", fd);
    ev_io_start(loop, clientArgs->readIO->asEvIO());
}

void SocksTunnelServer::remoteTimeoutCallback(struct ev_loop *loop, ev_timer *timer, int revents)
{
    RemoteServer *server = reinterpret_cast<RemoteServer *>(reinterpret_cast<IO *>(((char *)timer) - sizeof(ev_io))->server);
    QERROR("Time out....Fd:%d", server->writeIO->asEvIO()->fd);
    removeLocalServer(server->local);
    removeRemoteServer(server);
}
