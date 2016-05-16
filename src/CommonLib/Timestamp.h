#ifndef __H_TIMESTAMP__
#define __H_TIMESTAMP__

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#ifndef snprintf
#define snprintf _snprintf 
#endif
#else
#include <sys/time.h>
#endif

#include <stdint.h>
#include <string>
#include <algorithm>
#include <time.h>
#include <stdio.h>


namespace ts
{

    class Timestamp
    {
        const static uint32_t MILLISEC = 1000;
        const static uint32_t MICROSEC = 1000 * 1000;
    public:
        explicit Timestamp(const uint64_t &microts = 0) : microsec_(microts) {}
        Timestamp(const Timestamp &ts) { microsec_ = ts.getMicroSeconds(); }
        ~Timestamp() {}

    public:
        static Timestamp now();
        std::string toString() const;
        std::string toFormatedString(bool withMs = true) const;
        static Timestamp fromUnixTime(time_t ts) { return fromUnixTime(ts, 0); }
        static Timestamp fromUnixTime(time_t ts, uint32_t ms);
        void swap(Timestamp &ts) { std::swap(microsec_, ts.microsec_); }
        const uint64_t &getMicroSeconds() const { return microsec_; }
        Timestamp &addMilliSeconds(uint32_t millis);
        Timestamp &subMilliSeconds(uint32_t millis);
        Timestamp &addSeconds(uint32_t sec) { microsec_ += sec * MICROSEC; return *this; }
        Timestamp &subSeconds(uint32_t sec) { microsec_ -= sec * MICROSEC; return *this; }

    private:
        uint64_t microsec_;
    };

    bool operator==(const Timestamp &lhs, const Timestamp &rhs);
    bool operator<(const Timestamp &lhs, const Timestamp &rhs);
    bool operator<=(const Timestamp &lhs, const Timestamp &rhs);
    Timestamp operator+(const Timestamp &lhs, uint32_t offset);
    Timestamp &operator+=(Timestamp &lhs, uint32_t offset);
    Timestamp operator-(const Timestamp &lhs, uint32_t offset);
    Timestamp &operator-=(Timestamp &lhs, uint32_t offset);
    std::ostream &operator<<(std::ostream &os, Timestamp &ts);
}

#endif
