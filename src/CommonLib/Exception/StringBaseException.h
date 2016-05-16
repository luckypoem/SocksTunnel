#ifndef __H_STRINGBASEEXCEPTION__
#define __H_STRINGBASEEXCEPTION__

#include "Exception.h"
#include <string>

namespace except
{
    class StringBaseException : public Exception
    {
    protected:
        typedef std::string String;
        typedef ptr::SharedPtr<String> StringWrap;
    
    public:
        StringBaseException() {}
        ~StringBaseException() throw() {}

    protected:
        void setString(const String &str) { sw_ = StringWrap(new String(str)); }

    public:
        virtual char const* what() const throw() { return sw_.get() == NULL ? NULL : sw_->c_str(); }

    private:
        StringWrap sw_;
    };
}
    
#endif