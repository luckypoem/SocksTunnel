#ifndef __H_INIT_FAILED_EXCEPTION__
#define __H_INIT_FAILED_EXCEPTION__

#include "StringBaseException.h"

namespace except
{
    class InitFailedException : public StringBaseException
    {
    public:
        InitFailedException() { setString("init failed"); }
        InitFailedException(const String &str) { setString(str); }
        ~InitFailedException() throw() {}
    };

}

#endif