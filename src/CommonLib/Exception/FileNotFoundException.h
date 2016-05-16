#ifndef __H_FILENOTFOUNDEXCEPTION__
#define __H_FILENOTFOUNDEXCEPTION__

#include "StringBaseException.h"

namespace except
{
    class FileNotFoundException : public StringBaseException
    {
    public:
        FileNotFoundException() { setString("file not found"); }
        FileNotFoundException(const String &str) { setString(str); }
        ~FileNotFoundException() throw() {}
    };
}

#endif



