#ifndef __H_UNKNOWKEYEXCEPTION__
#define __H_UNKNOWKEYEXCEPTION__

#include "StringBaseException.h"

namespace except
{
    class UnKnowKeyException : public StringBaseException
    {
    public:
        UnKnowKeyException() { setString("unknow key"); }
        UnKnowKeyException(const String &str) { setString(str); }
        ~UnKnowKeyException() throw() {}
    };

}

#endif