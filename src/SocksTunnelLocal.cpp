//
// Created by sen on 16-5-13.
//

#include "SocksTunnelLocal.h"
#include "Utils/SettingUtils.h"
#include "Utils/RandomUtils.h"
#include "Crypto/CryptoHelper.h"

SocksTunnelLocal::SocksTunnelLocal()
{

}

SocksTunnelLocal::~SocksTunnelLocal()
{

}

void SocksTunnelLocal::start()
{
    int fd = createLocalServer(SettingUtils::newInstance().getLocalServer().c_str(), SettingUtils::newInstance().getLocalPort());
    LocalServer *server = new LocalServer(this);
    ev_io_init(server->readIO->asEvIO(), acceptCallback, fd, EV_READ);
    ev_io_start(getLoop(), server->readIO->asEvIO());
    ev_run(getLoop(), 0);
}

void SocksTunnelLocal::readCallback(struct ev_loop *loop, ev_io *args, int revents)
{
    LocalServer *server = static_cast<LocalServer *>(reinterpret_cast<IO *>(args)->server);
    EXIT_IF(server == NULL, "Can't cast to local server?");

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
        case LocalServer::Init:
        {
            bool hasMethod0x0 = false;

            //need more data;
            if(server->readIO->total < 2)
                return;
            char ver = server->readIO->buf[0];
            char methodCount = server->readIO->buf[1];
            //need more data;
            if(server->readIO->total - 2 < methodCount)
                return;
            QDEBUG("Fd:%d, Recv handshake, ver:%d, method count:%d, methods:", server->readIO->asEvIO()->fd, (int)ver, (int)methodCount);
            for(auto i = 0; i < methodCount; ++i)
            {
                QDEBUG("Method %d:%d", (int)(i + 1), static_cast<int>(server->readIO->buf[2 + i]));
                if(server->readIO->buf[2 + i] == 0x0)
                    hasMethod0x0 = true;
            }
            if(ver != 0x5 || !hasMethod0x0)
            {
                QERROR("Unsupport version or no support method, version:%d, has method 0x0:%d", ver, (int)hasMethod0x0);
                server->readIO->buf[1] = static_cast<char>(0xff);
            }
            else
            {
                server->readIO->buf[1] = 0x0;
                server->stage = LocalServer::HandShake;
            }
            QDEBUG("Fd:%d, Begin switch to write...", server->writeIO->asEvIO()->fd);
            //server->writeIO->copyBuffer(server->readIO->buf, 2);
            if(!sendAll(server->writeIO->asEvIO()->fd, server->readIO->buf, 2))
            {
                isError = true;
                QERROR("Send init message failed...Fd:%d", server->writeIO->asEvIO()->fd);
                break;
            }
            server->readIO->resetBuffer();
            QDEBUG("Fd:%d init success..begin to handshake...", server->writeIO->asEvIO()->fd);
            break;
        }
        case LocalServer::HandShake:
        {
            //need more data;
            if(server->readIO->total < 5)
                return;
            int totallen = 0;
            if(server->readIO->buf[3] == 0x01)
                totallen = 4 + 4 + 2;
            else if(server->readIO->buf[3] == 0x03)
                totallen = 4 + 1 + server->readIO->buf[4] + 2;
            else if(server->readIO->buf[3] == 0x04)
                totallen = 4 + 16 + 2;
            else
            {
                isError = true;
                QERROR("Unknow type?...type:%d, Fd:%d", server->readIO->buf[3], server->readIO->asEvIO()->fd);
                break;
            }
            //need more data;
            if(server->readIO->total < totallen)
                return;
            uint32_t total = 0;
            if(server->readIO->total != totallen)
            {
                QERROR("server->total != total len?, total:%d, totallen:%d", server->readIO->total, totallen)
                server->readIO->buf[std::max((int)server->readIO->total, totallen)] = '\0';
                QERROR("String:%s", server->readIO->buf);
            }

            QERROR_IF(server->remote != NULL, "In error state!!!! Fd:%d", server->readIO->asEvIO()->fd);


            const char *addr = NULL;
            int addrLen = 0;
            uint16_t port = 0;
            char addrType = server->readIO->buf[3];
            if(addrType == 0x03)
            {
                addrLen = server->readIO->buf[4];
                addr = server->readIO->buf + 5;
            }
            else
            {
                addr = server->readIO->buf + 4;
                addrLen = server->readIO->buf[3] == 0x01 ? 4 : 16;
            }
            port = ntohs(*(uint16_t *)(server->readIO->buf + totallen - 2));

            int remotefd = createRemoteServer(SettingUtils::newInstance().getRemoteServer().c_str(), (uint16_t)SettingUtils::newInstance().getRemotePort());
            //createRemoteServer(addr, addrLen, port, addrType);
            if(remotefd == -1)
            {
                QERROR("Create remote fd failed...");
                isError = true;
                break;
            }
            if(server->remote != NULL)
            {
                QERROR("Error here...remote is not null, fd:%d", server->readIO->asEvIO()->fd);
                isError = true;
                break;
            }

            server->remote = new RemoteServer(server->tunnel);
            server->remote->local = server;

            ev_io_init(server->remote->readIO->asEvIO(), remoteReadCallback, remotefd, EV_READ);
            ev_io_init(server->remote->writeIO->asEvIO(), remoteWriteCallback, remotefd, EV_WRITE);
            ev_timer_init(&server->remote->writeIO->timer, remoteTimeoutCallback, 60, 0);
            QDEBUG("Local server Fd:%d bind to remote Fd:%d", server->readIO->asEvIO()->fd, server->remote->writeIO->asEvIO()->fd);
            if(server->remote == NULL)
            {
                QDEBUG("Could not cast to remote server, connect failed?");
                isError = true;
                break;
            }
            //auth msg;
            AuthMsg msg;
            msg.addr.assign(addr, addrLen);
            msg.addrLen = addrLen;
            msg.addrType = addrType;
            msg.port = port;
            msg.user = SettingUtils::newInstance().getCurUser();
            msg.pwd = SettingUtils::newInstance().getCurPwd();
            String tmp;
            msg.encode(tmp);
            QDEBUG("Msg info:%s", msg.info().c_str());
            //====ENCRYPT====
            CryptoHelper::newInstance().encrypt(tmp, SettingUtils::newInstance().getPassword());

            server->remote->writeIO->appendInt((int)tmp.size());
            server->remote->writeIO->appendBuffer(tmp.c_str(), (int)tmp.size());
            QDEBUG("After append, remote write buf count:%d, Fd:%d", server->remote->writeIO->total, server->readIO->asEvIO()->fd);

            //begin to switch...
            ev_io_stop(server->tunnel->getLoop(), server->readIO->asEvIO());
            ev_io_start(server->tunnel->getLoop(), server->remote->writeIO->asEvIO());
            ev_timer_start(server->tunnel->getLoop(), &server->remote->writeIO->timer);
            //Create a fake response to client...
            server->readIO->buf[1] = 0x0;
            //send util success..
            QDEBUG("Begin to send fake response to Fd:%d...", server->readIO->asEvIO()->fd);
            if(!sendAll(server->readIO->asEvIO()->fd, server->readIO->buf, totallen))
            {
                QERROR("Send fake response failed....Fd:%d", server->writeIO->asEvIO()->fd);
                isError = true;
                break;
            }
            server->readIO->resetBuffer();
            server->stage = LocalServer::Connect;
            QDEBUG("Fd:%d handshake success.. Change to connect stage...", server->readIO->asEvIO()->fd);
            break;
        }
        case LocalServer::Connect:
        {
            QDEBUG("Fd:%d, In connect...", server->readIO->asEvIO()->fd);
            if(server->remote->writeIO->total != server->remote->writeIO->startPos)
            {
                QDEBUG("May not connect to remote? wait it..., remote fd:%d", server->remote->writeIO->asEvIO()->fd);
                return;
            }
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
                QDEBUG("Msg info:%s", msg.info().c_str());
                QDEBUG("Msg:%s", utils::StringUtils::toString(msg.data).c_str());
                //====ENCRYPT====
                CryptoHelper::newInstance().encrypt(tmp, SettingUtils::newInstance().getPassword());
                QDEBUG("Append %d byte to local write...Fd:%d", (int)tmp.size(), server->readIO->asEvIO()->fd);
                server->remote->writeIO->appendInt((int)tmp.size());
                server->remote->writeIO->appendBuffer(tmp.c_str(), tmp.size());
                sz += avaCount;
            }
            while(sz < server->readIO->total);
            server->readIO->resetBuffer();
            ev_io_stop(server->tunnel->getLoop(), server->readIO->asEvIO());
            ev_io_start(server->tunnel->getLoop(), server->remote->writeIO->asEvIO());
            break;
        }
        default:
            QDEBUG("Unknow stage:%d, Fd:%d", static_cast<int>(server->stage), server->readIO->asEvIO()->fd);
            isError = true;
            break;
    }

    if(isError)
    {
        closeLocalAndRemoteServer(server, server->remote);
    }
}

void SocksTunnelLocal::writeCallback(struct ev_loop *loop, ev_io *args, int revents)
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
            QDEBUG("Write error....");
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

void SocksTunnelLocal::acceptCallback(struct ev_loop *loop, ev_io *args, int revents)
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
    evutil_make_socket_nonblocking(fd);
    Server *clientArgs = new LocalServer(server->tunnel);
    ev_io_init(clientArgs->readIO->asEvIO(), readCallback, fd, EV_READ);
    ev_io_init(clientArgs->writeIO->asEvIO(), writeCallback, fd, EV_WRITE);
    QDEBUG("Begin to listen Fd:%d", fd);
    ev_io_start(loop, clientArgs->readIO->asEvIO());
}

void SocksTunnelLocal::remoteReadCallback(struct ev_loop *loop, ev_io *args, int revents)
{
    QDEBUG("Remote read....");
    RemoteServer *server = reinterpret_cast<RemoteServer *>(reinterpret_cast<IO *>(args)->server);
    bool isError = false;
    ssize_t count = read(server->readIO->asEvIO()->fd, server->readIO->buf + server->readIO->total, MAX_SERVER_BUF - server->readIO->total);
    if(count == 0 && server->readIO->total == 0)
    {
        isError = true;
        QDEBUG("Server close connect...Fd:%d", server->readIO->asEvIO()->fd);
        closeLocalAndRemoteServer(server->local, server);
        return;
    }
    if(count < 0)
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        QERROR("Socket error... Fd:%d", server->readIO->asEvIO()->fd);
        isError = true;
        closeLocalAndRemoteServer(server->local, server);
        return;
    }
    server->readIO->total += count;
    if(server->readIO->total < 4)
        return;
    int msgCount = 0;
    do
    {
        if(server->readIO->startPos + 4 >= server->readIO->total)
            break;
        int total = ntohl(*reinterpret_cast<int *>(server->readIO->buf + server->readIO->startPos));
        if(total <= 0)
        {
            isError = true;
            QERROR("Got a negative number... Fd:%d", server->readIO->asEvIO()->fd);
            break;
        }
        if(total > REAL_SERVER_BUF)
        {
            isError = true;
            QERROR("Got a unexpected total, total:%d, should less than:%d", total, REAL_SERVER_BUF);
            break;
        }
        //need more data;
        if(server->readIO->startPos + total + 4 > server->readIO->total)
        {
            if(msgCount > 0)
                break;
            else
                return;
        }
        //====DECRYPT====
        String tmp(server->readIO->buf + server->readIO->startPos, 4);
        String dataStr(server->readIO->buf + server->readIO->startPos + 4, total);
        CryptoHelper::newInstance().decrypt(dataStr, SettingUtils::newInstance().getPassword());
        tmp += dataStr;
        DataMsg msg;
        if(!msg.decode(tmp))
        {
            isError = true;
            QERROR("Decode msg failed, msg len:%d, Fd:%d", total, server->readIO->asEvIO()->fd);
            break;
        }
        if(!msg.check())
        {
            isError = true;
            QERROR("Msg check failed, msg len:%d, Fd:%d", total, server->readIO->asEvIO()->fd);
        }

        QDEBUG("Decode packet, data len:%d", (int)msg.data.size());
        server->local->writeIO->appendBuffer(msg.data.c_str(), msg.data.size());
        server->readIO->startPos += 4 + total;
        ++msgCount;

    }
    while(true);
    if(!isError)
    {
        server->readIO->compack();

        ev_io_stop(server->tunnel->getLoop(), server->readIO->asEvIO());
        ev_io_start(server->tunnel->getLoop(), server->local->writeIO->asEvIO());
        QDEBUG("Fd:%d stop read and change to write Fd:%d", server->readIO->asEvIO()->fd, server->local->writeIO->asEvIO()->fd);
    }
    if(isError)
    {
        closeLocalAndRemoteServer(server->local, server);
    }
}

void SocksTunnelLocal::remoteWriteCallback(struct ev_loop *loop, ev_io *args, int revents)
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
    if(server->writeIO->total == 0)
    {
        //closeLocalAndRemoteServer(server->local, server);
        QDEBUG("Fd:%d, No data to write...may test connect, am i connected:%d?", server->writeIO->asEvIO()->fd, server->stage == RemoteServer::Connected);
        ev_io_stop(server->tunnel->getLoop(), server->writeIO->asEvIO());
        ev_io_start(server->tunnel->getLoop(), server->local->readIO->asEvIO());
        return;
    }
    ssize_t count = write(server->writeIO->asEvIO()->fd, server->writeIO->buf + server->writeIO->startPos, server->writeIO->total - server->writeIO->startPos);
    do
    {
        if(count <= 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
                return;
            isError = true;
            break;
        }
        server->writeIO->startPos += count;
        if(server->writeIO->startPos == server->writeIO->total)
        {
            QDEBUG("Write to remote success...count:%d, Fd:%d", server->writeIO->total, server->writeIO->asEvIO()->fd);
            server->writeIO->resetBuffer();
            ev_io_stop(server->tunnel->getLoop(), server->writeIO->asEvIO());
            ev_io_start(server->tunnel->getLoop(), server->local->readIO->asEvIO());
        }
    }
    while(false);


    if(isError)
    {
        closeLocalAndRemoteServer(server->local, server);
    }
}

void SocksTunnelLocal::remoteTimeoutCallback(struct ev_loop *loop, ev_timer *timer, int revents)
{
    RemoteServer *server = reinterpret_cast<RemoteServer *>(reinterpret_cast<IO *>(((char *)timer) - sizeof(ev_io))->server);
    QERROR("Time out....Fd:%d", server->writeIO->asEvIO()->fd);

    removeLocalServer(server->local);
    removeRemoteServer(server);
}
