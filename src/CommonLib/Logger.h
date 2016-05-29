#ifndef __H_LOGGER__
#define __H_LOGGER__

#include "Singleton.h"
#include <string>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <assert.h>

typedef std::string String;

namespace loggable
{
    namespace debugger
    {

        class Logger : public utils::ISingleton<Logger>
        {
        public:
            Logger() : logger_(NULL) {}

        public:
            void init(const String &file);
            log4cpp::Category *getLogger();

        private:
            log4cpp::Category *logger_;
        };



#ifndef NDEBUG
#define QDEBUG(fmt, ...) \
{\
    {\
        if(loggable::debugger::Logger::newInstance().getLogger() != NULL) \
            loggable::debugger::Logger::newInstance().getLogger()->debug("[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }\
}
#define QDEBUG_EX(expression, fmt, ...) \
{\
    if(!(expression)) \
    {\
        QDEBUG(fmt, ##__VA_ARGS__); \
    }\
}
#define QDEBUG_IF(expression, fmt, ...) \
{\
    QDEBUG_EX(!(expression), fmt, ##__VA_ARGS__); \
}
#else
#define QDEBUG(fmt, ...) {}
#define QDEBUG_EX(expression, fmt, ...) {}
#define QDEBUG_IF(expression, fmt, ...) {}
#endif
#define QERROR(fmt, ...) \
{\
    {\
        if(loggable::debugger::Logger::newInstance().getLogger() != NULL) \
            loggable::debugger::Logger::newInstance().getLogger()->error("[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }\
}
#define QERROR_IF(expression, fmt, ...) \
{\
    if((expression)) \
    {\
        QERROR(fmt, ##__VA_ARGS__); \
    }\
}

#define EXIT_IF(expression, fmt, ...)\
{\
    if((expression)) \
    {\
        QERROR(fmt, ##__VA_ARGS__); \
        exit(0); \
    }\
}
    }
}


#endif