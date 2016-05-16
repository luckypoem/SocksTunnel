//
// Created by sen on 16-5-14.
//

#ifndef SOCKSTUNNELLOCAL_BUFFER_H
#define SOCKSTUNNELLOCAL_BUFFER_H

#include <string>
#include <netinet/in.h>
#include "../CommonLib/NoCopyable.h"

typedef std::string String;

#define __CHECK_LENGTH__(count) {\
    if(index_ + (count) > str_.size())\
    {\
        return false;\
    }\
}

class BufferReader : utils::NoCopyable
{
public:
    BufferReader(const String &str) : str_(str), index_(0) {}

public:
    bool readInt(int &val);
    bool readShort(short &val);
    bool readByte(char &val);
    bool readBool(bool &val);
    bool skip(int count);
    bool readString(String &buf, int count);
    bool readString(char *buf, int count);

public:
    uint32_t getIndex() const { return index_; }
    bool hasSpare() const { return index_ != str_.size(); }
    String getSpare();

private:
    const char *current();

private:
    const String &str_;
    uint32_t index_;
};


#endif //SOCKSTUNNELLOCAL_BUFFER_H
