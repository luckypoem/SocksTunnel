//
// Created by sen on 16-5-14.
//

#include "SettingUtils.h"

void SettingUtils::init(const String &file)
{
    json_ = parser::JsonParser::parseFromFile(file);
    QDEBUG_IF(json_.isNil(), "Parse json file failed, check it please, file:%s", file.c_str());
}

const String &SettingUtils::getPassword() const
{
    __CHECK_OBJECT_AND_RETURN_STRING__(json_, "password", NULL_STRING());
}

bool SettingUtils::checkUser(const String &user, const String pwd) const
{
    bool ret = false;
    do
    {
        if(json_.isNil())
        {
            QERROR("Json is nil, may init failed or may not init...");
            break;
        }

        const auto &usermap = json_.objectValue().at("user");
        if(usermap.isNil())
        {
            QERROR("I could not found user map...");
            break;
        }
        auto iter = usermap.objectValue().find(user);
        if(iter == usermap.objectValue().end())
            break;
        ret = (iter->second == pwd);
    }
    while(false);
    return ret;
}

const String &SettingUtils::getRemoteServer() const
{
    __CHECK_OBJECT_AND_RETURN_STRING__(json_, "remote_server", NULL_STRING());
}

const String &SettingUtils::getLocalServer() const
{
    __CHECK_OBJECT_AND_RETURN_STRING__(json_, "local_server", NULL_STRING());
}

uint32_t SettingUtils::getLocalPort() const
{
    __CHECK_OBJECT_AND_RETURN_INT__(json_, "local_port", 9000);
}

uint32_t SettingUtils::getRemotePort() const
{
    __CHECK_OBJECT_AND_RETURN_INT__(json_, "remote_port", 9001);
}

void SettingUtils::getMethod(std::vector<String> &ms) const
{
    do
    {
        if(json_.isNil())
        {
            QERROR("Json is nil....");
            break;
        }
        if(json_.objectValue().find("method") == json_.objectValue().end())
        {
            QERROR("Method item not found...");
            break; 
        }

        const auto &vec = json_.objectValue().at("method").arrayValue();
        if(vec.size() == 0)
            QERROR("No method found...");
        for(auto &item : vec)
            ms.push_back(item.stringValue());
    }
    while(false);
}

const String &NULL_STRING()
{
    static String ret = "";
    return ret;
}

const String &SettingUtils::getCurUser() const
{
    __CHECK_OBJECT_AND_RETURN_STRING__(json_, "current_user", NULL_STRING());
}

const String &SettingUtils::getCurPwd() const
{
    __CHECK_OBJECT_AND_RETURN_STRING__(json_, "current_pwd", NULL_STRING());
}

uint32_t SettingUtils::getAuthTimeout() const
{
    __CHECK_OBJECT_AND_RETURN_INT__(json_, "auth_timeout", 30);
}
