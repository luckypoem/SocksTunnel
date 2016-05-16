#ifndef __H_PARSEREXCEPTION__
#define __H_PARSEREXCEPTION__

#include "StringBaseException.h"

namespace except
{
    class ParseException : public StringBaseException
    {
    public:
        ParseException() { setString("parse failed"); }
        ParseException(const String &str) { setString(str); }
        ~ParseException() throw() {}
    };

}
    
#endif