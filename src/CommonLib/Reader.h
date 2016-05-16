#ifndef __H_READER__
#define __H_READER__

#include <stdint.h>
#include "SharedPtr.h"
#if __cplusplus < 201103L
typedef bool(*CharFilter)(char);
#else
#include <functional>
typedef std::function<bool(char)> CharFilter;
#endif

#include <fstream>
#include <assert.h>

namespace io
{
    typedef std::ifstream InStream;
    class Readable
    {
    public:
        virtual ~Readable() {}
        
    public:
        virtual char readChar() = 0;
        virtual bool isEnd() = 0;
        virtual char top() = 0;
        virtual bool skip(uint32_t n);
    };

    typedef ptr::SharedPtr<Readable> ReadableWrap;

    class Reader : public Readable
    {
        
    public:
        Reader() :cur_(0) {}
        Reader(ReadableWrap readable) : cur_(0) { setReadableObject(readable); }
        ~Reader() {}

    public:
        char readChar();
        char readChar(const CharFilter &filter);
        bool isEnd();
        char top();
        bool skip(uint32_t n = 1);

    public:
        void skip(const CharFilter &filter);
        int32_t current() const { return cur_; }

    public:
        void setReadableObject(ReadableWrap readable) { rw_ = readable; cur_ = 0; }

    protected:
        int32_t incrCurrent(int32_t count = 1) { return cur_ += count; }
        ReadableWrap &getReader() { return rw_; }

    private:
        ReadableWrap rw_;
        int32_t cur_;
    };

    class StreamReader : public Readable
    {
    public:
        static const int MAX_STREAM_BUFFER = 2048;
        StreamReader(InStream &in) : in_(in), autoClose_(true), bufferIndex_(0), totalCount_(0) {}
        ~StreamReader() { if (isAutoClose()) { in_.close(); } }

    public:
        char readChar();
        bool isEnd();
        char top();

    public:
        void setAutoClose(bool isAuto) { autoClose_ = isAuto; }
        bool isAutoClose() { return autoClose_; }

    protected:
        InStream &getStream() { return in_; }
        char *getBuffer() { return buffer_; }
        void setBufferIndex(int32_t index) { bufferIndex_ = index; }
        void incrBufferIndex(int32_t count = 1) { bufferIndex_ += count; }
        int32_t curBufferIndex() const { return bufferIndex_; }
        void setTotalCount(int32_t count) { totalCount_ = count; }
        int32_t getTotalCount() const { return totalCount_; }
        void adjust();

    private:
        StreamReader(StreamReader &sr) :in_(sr.in_) {}
        StreamReader &operator=(StreamReader &) { return *this; }

    private:
        InStream &in_;
        bool autoClose_;
        char buffer_[MAX_STREAM_BUFFER];
        int32_t bufferIndex_;
        int32_t totalCount_;
    };

    class StringReader : public Readable
    {
    public:
        typedef std::string String;
        StringReader(const String &str) : str_(str), index_(0) {}
        StringReader(const String &str, uint32_t index) : str_(str), index_(index) {}

    public:
        char readChar();
        bool isEnd();
        char top();

    protected:
        const String &getString() { return str_; }
        uint32_t incrIndex() { return ++index_; }
        uint32_t incrIndex(int count) { index_ += count; return index_; }
        uint32_t curIndex() { return index_; }

    private:
        const String &str_;
        uint32_t index_;
    };
}

#endif
