#include "Timestamp.h"

namespace ts
{
    Timestamp Timestamp::now()
    {
#ifdef _WIN32
        SYSTEMTIME st;
        GetLocalTime(&st);
        struct tm gm = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1, st.wYear - 1900, st.wDayOfWeek, 0, 0 };
        return Timestamp(static_cast<uint64_t>(mktime(&gm)) * MICROSEC + st.wMilliseconds * MILLISEC);
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm gm;
        localtime_r(&tv.tv_sec, &gm);
        return Timestamp(static_cast<uint64_t>(mktime(&gm)) * MICROSEC + tv.tv_usec);
#endif
        return Timestamp();
    }

    std::string Timestamp::toString() const
    {
        char buf[30], *ptr = NULL;
        size_t sz = 0;
        ptr = buf;
        uint32_t ms = getMicroSeconds() % MICROSEC;
        uint32_t highSec = static_cast<uint32_t>((getMicroSeconds() / MICROSEC) / MICROSEC);
        uint32_t lowSec = static_cast<uint32_t>((getMicroSeconds() / MICROSEC) % MICROSEC);
        if(highSec != 0)
        {
            sz = snprintf(ptr, buf + sizeof(buf) - ptr, "%u", highSec);
            ptr += sz;
        }
        sz = snprintf(ptr, buf + sizeof(buf) - ptr, "%06u", lowSec);
        ptr += sz;
        snprintf(ptr, buf + sizeof(buf) - ptr, ".%06u", ms);
        return buf;

    }

    std::string Timestamp::toFormatedString(bool withMs) const
    {
        time_t t = static_cast<time_t>(getMicroSeconds() / MICROSEC);
        uint32_t ms = static_cast<uint32_t>(getMicroSeconds() % MICROSEC);
        struct tm gm;
        char buf[30];
#ifdef _WIN32
        localtime_s(&gm, &t);
#else
        localtime_r(&t, &gm);
#endif
        size_t sz = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &gm);
        if (withMs)
            snprintf(buf + sz, sizeof(buf) - sz, ".%06u", ms);
        return buf;
    }

    Timestamp Timestamp::fromUnixTime(time_t ts, uint32_t ms)
    {
        return Timestamp(static_cast<uint64_t>(ts * MICROSEC + ms));
    }

    Timestamp &Timestamp::addMilliSeconds(uint32_t millis)
    {
        microsec_ += millis * MILLISEC;
        return *this;
    }

    Timestamp &Timestamp::subMilliSeconds(uint32_t millis)
    {
        microsec_ -= millis * MILLISEC;
        return *this;
    }

    bool operator==(const Timestamp & lhs, const Timestamp & rhs)
    {
        return lhs.getMicroSeconds() == rhs.getMicroSeconds();
    }

    bool operator<(const Timestamp & lhs, const Timestamp & rhs)
    {
        return lhs.getMicroSeconds() < rhs.getMicroSeconds();
    }

    bool operator<=(const Timestamp & lhs, const Timestamp & rhs)
    {
        return lhs.getMicroSeconds() <= rhs.getMicroSeconds();
    }

    Timestamp operator+(const Timestamp & lhs, uint32_t offset)
    {
        return Timestamp(lhs).addSeconds(offset);
    }

    Timestamp &operator+=(Timestamp & lhs, uint32_t offset)
    {
        lhs.addSeconds(offset);
        return lhs;
    }

    Timestamp operator-(const Timestamp & lhs, uint32_t offset)
    {
        return Timestamp(lhs).subSeconds(offset);
    }

    Timestamp &operator-=(Timestamp & lhs, uint32_t offset)
    {
        lhs.subSeconds(offset);
        return lhs;
    }

    std::ostream &operator<<(std::ostream &os, Timestamp &ts)
    {
        os << ts.toString();
        return os;
    }

}
