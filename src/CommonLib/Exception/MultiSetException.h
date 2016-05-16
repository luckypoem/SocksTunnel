#ifndef __H_MULTISETEXCEPTION__
#define __H_MULTISETEXCEPTION__

#include "StringBaseException.h"

namespace except
{
    class MultiSetException : public StringBaseException
    {
    public:
        MultiSetException() { setString("couldn't set twice"); }
        MultiSetException(const String &str) { setString(str); }
        ~MultiSetException() throw() {}
    };

}

#endif