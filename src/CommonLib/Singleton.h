#ifndef __H_ISINGLETON__
#define __H_ISINGLETON__

#include "NoCopyable.h"

namespace utils
{
    template<class Object>
    class ISingleton : public utils::NoCopyable
    {
    public:
        static Object &newInstance()
        {
            return getObject();
        }

    private:
        static Object &getObject()
        {
            static Object obj;
            return obj;
        }
    };
}
#endif
