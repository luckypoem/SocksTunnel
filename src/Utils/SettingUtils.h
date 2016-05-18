//
// Created by sen on 16-5-14.
//

#ifndef SOCKSTUNNELLOCAL_KEYMANAGER_H
#define SOCKSTUNNELLOCAL_KEYMANAGER_H


#include "../CommonLib/Singleton.h"
#include "../CommonLib/Json.h"
#include <string>
#include "../CommonLib/Logger.h"
#include <vector>

typedef std::string String;
using namespace loggable::debugger;

class SettingUtils : public utils::ISingleton<SettingUtils>
{
public:
    SettingUtils() {}
    ~SettingUtils() {}

public:
    void init(const String &file);

public:
    const String &getPassword() const;
    bool checkUser(const String &user, const String pwd) const;
    const String &getRemoteServer() const;
    const String &getLocalServer() const;
    uint32_t getLocalPort() const;
    uint32_t getRemotePort() const;
    void getMethod(std::vector<String> &ms) const;
    const String &getCurUser() const;
    const String &getCurPwd() const;
    uint32_t getAuthTimeout() const;


private:
    parser::Json json_;
};

static const String &NULL_STRING();


#define __CHECK_OBJECT_AND_RETURN__(js, item, value) \
{\
    if(js.isNil()) \
    {\
        QERROR("Json is nil, may set it first...");\
        return value;\
    } \
    if(js.objectValue().find(item) == js.objectValue().end())\
    {\
        QDEBUG("Item:%s not found...", item);\
        return value;\
    }\
}

#define __CHECK_OBJECT_AND_RETURN_STRING__(js, item, value)\
{\
    __CHECK_OBJECT_AND_RETURN__(js, item, value); \
    return js.objectValue().at(item).stringValue();\
}

#define __CHECK_OBJECT_AND_RETURN_INT__(js, item, value) \
{\
    __CHECK_OBJECT_AND_RETURN__(js, item, value); \
    return js.objectValue().at(item).intValue();\
}


#endif //SOCKSTUNNELLOCAL_KEYMANAGER_H
