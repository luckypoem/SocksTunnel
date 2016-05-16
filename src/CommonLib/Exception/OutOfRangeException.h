#ifndef __H_OUTOFRANGEEXCEPTION__
#define __H_OUTOFRANGEEXCEPTION__

#include "StringBaseException.h"

namespace except
{
    class OutOfRangeException : public StringBaseException
    {
    public:
        OutOfRangeException() { setString("index out of range"); }
        OutOfRangeException(const String &str) { setString(str); }
        ~OutOfRangeException() throw() {}
    };
}

#endif