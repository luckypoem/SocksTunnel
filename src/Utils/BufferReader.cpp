//
// Created by sen on 16-5-14.
//


#include <string.h>
#include "BufferReader.h"

bool BufferReader::readInt(int &val)
{
    __CHECK_LENGTH__(4);
    val = ntohl(*(int *)current());
    index_ += 4;
    return true;
}

bool BufferReader::readShort(short &val)
{
    __CHECK_LENGTH__(2);
    val = ntohs(*(short *)current());
    index_ += 2;
    return true;
}

bool BufferReader::readByte(char &val)
{
    __CHECK_LENGTH__(1);
    val = *current();
    ++index_;
    return true;
}

bool BufferReader::readBool(bool &val)
{
    __CHECK_LENGTH__(1);
    if(*current() == 0)
        val = false;
    if(*current() == 1)
        val = true;
    ++index_;
    return true;
}

bool BufferReader::skip(int count)
{
    __CHECK_LENGTH__(count);
    index_ += count;
    return true;
}

bool BufferReader::readString(String &buf, int count)
{
    __CHECK_LENGTH__(count);
    buf.assign(current(), count);
    index_ += count;
    return true;
}

bool BufferReader::readString(char *buf, int count)
{
    __CHECK_LENGTH__(count);
    strncpy(buf, current(), count);
    index_ += count;
    return true;
}

const char *BufferReader::current()
{
    return str_.c_str() + index_;
}

String BufferReader::getSpare()
{
    String ret = str_.substr(index_);
    index_ = str_.size();
    return ret;
}
