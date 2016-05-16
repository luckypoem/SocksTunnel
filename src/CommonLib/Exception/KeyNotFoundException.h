#ifndef __H_KEYNOTFOUNDEXCEPTION__
#define __H_KEYNOTFOUNDEXCEPTION__

#include "StringBaseException.h"

namespace except
{
    class KeyNotFoundException : public StringBaseException
    {
    public:
        KeyNotFoundException() { setString("key not found"); }
        KeyNotFoundException(const String &str) { setString(str); }
        ~KeyNotFoundException() throw() {}
    };
}

#endif