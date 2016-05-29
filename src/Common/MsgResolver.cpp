//
// Created by sen on 16-5-13.
//

#include "MsgResolver.h"

/*
 *   auth msg: |total:4|randLen:1|randChar:-|type:1(0x1)|userLen:1|user:-|pwdLen:1|pwd:-|carrayLen:2|carray:-|
 *   data msg: |total:4|randLen:1|randChar:-|type:1(0x2)|dataLen:2|data:-|
 *
 * */

bool AuthMsg::encode(String &buf)
{
    //create rnd char;
    int rndSz = RandomUtils::randInt(5, 100);
    rndChar = std::move(RandomUtils::randString(rndSz));
    type = 0x1;
    __CHECK_STRING_SIZE__(user, 6, 16);
    __CHECK_STRING_SIZE__(pwd, 6, 32);

    BufferWriter writer(buf);
    writer.writeByte((char)rndChar.size());
    writer.writeString(rndChar);
    writer.writeByte(type);
    writer.writeByte((char)user.size());
    writer.writeString(user);
    writer.writeByte((char)pwd.size());
    writer.writeString(pwd);
    writer.writeByte(addrType);
    writer.writeByte(addrLen);
    writer.writeString(addr);
    writer.writeShort(port);
    return true;
}

bool AuthMsg::decode(const String &msg)
{
    BufferReader reader(msg);
    __CHECK_READER__(reader.readInt(total));
    char rndLen = 0;
    __CHECK_READER__(reader.readByte(rndLen));
    __CHECK_READER__(reader.readString(rndChar, (unsigned char)rndLen));
    __CHECK_READER__(reader.readByte(type));
    char userLen = 0;
    __CHECK_READER__(reader.readByte(userLen));
    __CHECK_READER__(reader.readString(user, (unsigned char)userLen));
    char pwdLen = 0;
    __CHECK_READER__(reader.readByte(pwdLen));
    __CHECK_READER__(reader.readString(pwd, (unsigned char)pwdLen));
    __CHECK_READER__(reader.readByte(addrType));
    __CHECK_READER__(reader.readByte(addrLen));
    __CHECK_READER__(reader.readString(addr, (unsigned char)addrLen));
    __CHECK_READER__(reader.readShort(port));
    return true;
}

bool AuthMsg::check() const
{
    __CHECK_INT_SIZE__(total, 0, INT_MAX);
    __CHECK_STRING_SIZE__(rndChar, 5, 100);
    __CHECK_INT_SIZE__(type, 1, 1);
    __CHECK_STRING_SIZE__(user, 6, 16);
    __CHECK_STRING_SIZE__(pwd, 6, 32);
    if(addrType != 0x1 && addrType != 0x3 && addrType != 0x4)
        return false;
    __CHECK_STRING_SIZE__(addr, 0, 255);
    return true;
}

bool DataMsg::encode(String &buf)
{
    type = 0x2;
    int rndLen = RandomUtils::randInt(5, 10);
    rndChar = std::move(RandomUtils::randString(rndLen));
    __CHECK_STRING_SIZE__(data, 0, 4096);

    BufferWriter writer(buf);
    //writer.writeInt(total);
    writer.writeByte(static_cast<char>(rndChar.size()));
    writer.writeString(rndChar);
    writer.writeByte(type);
    unsigned long  crc = crc32(0L, Z_NULL, 0);
    dataCheck = (int32_t)crc32(crc, (const unsigned char*)data.c_str(), data.size());
    writer.writeInt(dataCheck);
    writer.writeShort(static_cast<short>(data.size()));
    writer.writeString(data);
    return true;
}

bool DataMsg::decode(const String &msg)
{
    BufferReader reader(msg);
    __CHECK_READER__(reader.readInt(total));
    char rndLen = 0;
    __CHECK_READER__(reader.readByte(rndLen));
    __CHECK_READER__(reader.readString(rndChar, (unsigned char)rndLen));
    __CHECK_READER__(reader.readByte(type));
    __CHECK_READER__(reader.readInt(dataCheck));
    short dataLen = 0;
    __CHECK_READER__(reader.readShort(dataLen));
    __CHECK_READER__(reader.readString(data, dataLen));
    return true;
}

bool DataMsg::check() const
{
    __CHECK_INT_SIZE__(total, 0, INT_MAX);
    __CHECK_STRING_SIZE__(rndChar, 5, 10);
    __CHECK_INT_SIZE__(type, 2, 2);
    __CHECK_STRING_SIZE__(data, 0, 8096);
    unsigned long  crc = crc32(0L, Z_NULL, 0);
    int32_t dCheck = (int32_t)crc32(crc, (const unsigned char*)data.c_str(), data.size());
    __CHECK_INT_SIZE__(dataCheck, dCheck, dCheck);
    return true;
}

String AuthMsg::info() const
{
    String ret("AuthMsg:[Rand count:");
    ret.append(std::to_string(rndChar.size()));
    ret.append(" Type:");
    ret.append(std::to_string((int)type));
    ret.append(" User:");
    ret.append(user);
    ret.append(" Pwd:");
    ret.append(pwd);
    ret.append(" AddrType:");
    ret.append(std::to_string((int)addrType));
    ret.append(" AddrLen:");
    ret.append(std::to_string((int)addrLen));
    ret.append(" Port:");
    ret.append(std::to_string(port));
    ret.append("]");
    return ret;
}

String DataMsg::info() const
{
    String ret("DataMsg:[Rand count:");
    ret.append(std::to_string(rndChar.size()));
    ret.append(" Type:");
    ret.append(std::to_string((int)type));
    ret.append(" Data check:");
    ret.append(std::to_string(dataCheck));
    ret.append(" Data len:");
    ret.append(std::to_string(data.size()));
    ret.append("]");
    return ret;
}
