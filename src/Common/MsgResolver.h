//
// Created by sen on 16-5-13.
//

#ifndef SOCKSTUNNEL_MSGRESOLVER_H
#define SOCKSTUNNEL_MSGRESOLVER_H


#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <limits.h>
#include "../CommonLib/NoCopyable.h"
#include "../Utils/BufferReader.h"
#include "../CommonLib/Logger.h"
#include "../Utils/BufferWriter.h"
#include "../Utils/RandomUtils.h"
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <zlib.h>

typedef std::string String;

using namespace loggable::debugger;


#define __CHECK_INT_SIZE__(intV, left, right) \
{\
    if((intV) < (left) || (intV) > (right)) \
    {\
        QERROR("Check Failed, Tag:"#intV)\
        return false;\
    }\
}

#define __CHECK_STRING_SIZE__(str, left, right) \
{\
    if((str.size()) < (left) || (str.size()) > (right))\
    {\
        QERROR("Check Failed, Tag:"#str)\
        return false;\
    }\
}

#define __CHECK_READER__(express) {\
    if(!(express))\
    {\
        QERROR("Check failed, express:"#express)\
        return false;\
    }\
}

struct AuthMsg
{
public:
    AuthMsg() : type(0x1) {}
    int total;
    String rndChar;  //5~500byte  //uint8_t
    char type;    //0x1
    String user;     //6~16byte  //uint8_t
    String pwd;      //6~32byte  //uint8_t
    char addrType;   //0x1, 0x3, 0x4
    char addrLen;    //0~255
    String addr;     //0~255
    short port;

public:
    String info() const;
    bool check() const;
    bool encode(String &buf);
    bool decode(const String &msg);
};

struct DataMsg
{
public:
    DataMsg() : type(0x2)
    {
        dataCheck = 0;
    }
    int32_t total;
    String rndChar;   //5~10byte //uint8
    char type;     //0x2
    int32_t dataCheck; //crc check
    String data;      //0~2048 //uint16_t

public:
    String info() const;
    bool check() const;
    bool encode(String &buf);
    bool decode(const String &msg);
};


#endif //SOCKSTUNNEL_MSGRESOLVER_H
