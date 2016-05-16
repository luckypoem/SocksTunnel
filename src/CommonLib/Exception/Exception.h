#ifndef __H_EXCEPTION__
#define __H_EXCEPTION__

#include <exception>
#include <string>
#include "../SharedPtr.h"

namespace except
{
    class Exception : public std::exception
    {
    public:
        Exception() {}
        virtual ~Exception() throw() {}

    public:
        virtual const char *what() const throw() = 0;
    };
}

#endif

