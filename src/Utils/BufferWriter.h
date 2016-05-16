//
// Created by sen on 16-5-14.
//

#ifndef SOCKSTUNNELLOCAL_BUFFERWRITER_H
#define SOCKSTUNNELLOCAL_BUFFERWRITER_H

#include <string>

typedef std::string String;

class BufferWriter
{
public:
    BufferWriter(String &buf);
    void writeInt(int val);
    void writeShort(short val);
    void writeByte(char val);
    void writeBool(bool val);
    void writeString(const char *str, int count);
    void writeString(const String &str);
    void writeString(const String &str, int count);

private:
    String &buf_;
};


#endif //SOCKSTUNNELLOCAL_BUFFERWRITER_H
