#include "Reader.h"

namespace io
{

    bool Readable::skip(uint32_t n)
    {
        while (!isEnd() && n)
        {
            readChar();
            --n;
        }
        return true;
    }

    char Reader::readChar()
    {
        incrCurrent();
        return getReader()->readChar();
    }

    char Reader::readChar(const CharFilter & filter)
    {
        char c;
        while (filter(c = readChar())) {}
        return c;
    }

    bool Reader::isEnd()
    {
        return getReader()->isEnd();
    }

    char Reader::top()
    {
        return getReader()->top();
    }

    bool Reader::skip(uint32_t n)
    {
        incrCurrent(n);
        return getReader()->skip(n);
    }

    void Reader::skip(const CharFilter & filter)
    {
        while (!isEnd() && filter(top()))
            skip(1);
    }

    char StreamReader::readChar()
    {
        adjust();
        char c = getBuffer()[curBufferIndex()];
        incrBufferIndex(1);
        return c;
    }

    bool StreamReader::isEnd()
    {
        return getStream().eof() && curBufferIndex() == getTotalCount();
    }

    char StreamReader::top()
    {
        adjust();
        return getBuffer()[curBufferIndex()];
    }

    void StreamReader::adjust()
    {
        if ((0 == curBufferIndex() && 0 == getTotalCount()) || curBufferIndex() == getTotalCount())
        {
            getStream().read(getBuffer(), MAX_STREAM_BUFFER);
            setTotalCount(static_cast<int32_t>(getStream().gcount()));
            setBufferIndex(0);
        }
    }

    char StringReader::readChar()
    {
        char c = getString().at(curIndex());
        incrIndex();
        return c;
    }

    bool StringReader::isEnd()
    {
        return curIndex() == getString().size();
    }

    char StringReader::top()
    {
        return getString().at(curIndex());
    }

}
