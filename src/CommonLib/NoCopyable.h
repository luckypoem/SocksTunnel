#ifndef __H_NOCOPYABLE__
#define __H_NOCOPYABLE__

namespace utils
{

    class NoCopyable
    {
    public:
        NoCopyable() {}
        ~NoCopyable() {}

    protected:
        NoCopyable(const NoCopyable &) {}
        NoCopyable &operator=(const NoCopyable &) { return *this; }
    };

}

#endif