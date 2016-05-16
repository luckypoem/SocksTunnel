//
// Created by sen on 16-5-14.
//

#include <netinet/in.h>
#include "BufferWriter.h"

BufferWriter::BufferWriter(String &buf) : buf_(buf)
{

}

void BufferWriter::writeInt(int val)
{
    int tmp = htonl(val);
    buf_.append((char *)&tmp, 4);
}

void BufferWriter::writeShort(short val)
{
    short tmp = htons(val);
    buf_.append((char *)&tmp, 2);
}

void BufferWriter::writeByte(char val)
{
    buf_.push_back(val);
}

void BufferWriter::writeBool(bool val)
{
    buf_.push_back(val == true ? 1 : 0);
}

void BufferWriter::writeString(const char *str, int count)
{
    buf_.append(str, count);
}

void BufferWriter::writeString(const String &str)
{
    buf_.append(str);
}

void BufferWriter::writeString(const String &str, int count)
{
    writeString(str.c_str(), count);
}
