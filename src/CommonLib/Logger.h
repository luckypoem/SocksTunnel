#ifndef __H_LOGGER__
#define __H_LOGGER__

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include "Singleton.h"
#include "StringUtils.h"
#include <map>
#include "Json.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "Mutex.h"
#include "StringUtils.h"
#include "Timestamp.h"
#include "Exception/UnKnowKeyException.h"
#include "Exception/FileNotFoundException.h"
#include "Exception/MultiSetException.h"


namespace loggable
{
    namespace debugger
    {
        thread::mutex::Mutex &getMutex();
#ifndef NDEBUG
#define QDEBUG(fmt, ...) \
{\
    {\
        thread::mutex::ScopeLock<thread::mutex::Mutex> mlock(loggable::debugger::getMutex());\
        fprintf(stdout, "[DEBUG][%s][%s:%s:%d] ", ts::Timestamp::now().toFormatedString(true).c_str(), __FILE__, __FUNCTION__, __LINE__);\
        fprintf(stdout, fmt"\n", ##__VA_ARGS__);\
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
        thread::mutex::ScopeLock<thread::mutex::Mutex> mlock(loggable::debugger::getMutex());\
        fprintf(stderr, "[ERROR][%s][%s:%s:%d] ", ts::Timestamp::now().toFormatedString(true).c_str(), __FILE__, __FUNCTION__, __LINE__);\
        fprintf(stderr, fmt"\n", ##__VA_ARGS__);\
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


    utils::String getLogTime();

    namespace easy_log
    {
        using utils::StringUtils;
        using utils::String;
        using thread::mutex::ScopeLock;
        using thread::mutex::Mutex;

        enum LogLevel { Debug = 0, Info, Warn, Error, Fatal };

        class Logger : public utils::NoCopyable
        {
        public:
            Logger() {}
            Logger(const String &file) {}
            ~Logger();

        public:
            bool isInited() const { return inited_; }
            bool isLogScreen() const { return logScreen_; }
            void init(const String &file);
            FILE *getLogStream(LogLevel lv);
            //void append(LogLevel lv, const char *file, const char *func, const uint32_t &line, const char *fmt, const char *msg);
            LogLevel getLogLevel() const { return lv_; }
            static const String &logLevelToString(LogLevel lv);
            Mutex &getMutex() { return mu_; }

        private:
            typedef std::map<LogLevel, FILE *> LogMap;
            Mutex mu_;
            LogMap map_;
            bool inited_;
            bool logScreen_;
            LogLevel lv_;

        protected:
            LogMap &getLogMap() { return map_; }
            const LogMap &getLogMap() const { return map_; }
            void setInit(bool inited) { inited_ = inited; }
            void setLogScreen(bool value) { logScreen_ = value; }
            void setLogLevel(LogLevel lv) { lv_ = lv; }
            void setLogStream(LogLevel lv, const String &file);
            void releaseLogMap();
        };

        typedef utils::ISingleton<Logger> Log;
#define LOG_COMMON_THREAD_SAFE_EASY(lv, fmt, ...) \
{\
    {\
        ScopeLock<Mutex> lock(Log::newInstance().getMutex()); \
        if (lv >= Log::newInstance().getLogLevel())\
        {\
            FILE *stream = Log::newInstance().getLogStream(Debug); \
            fprintf(stream, "[%s][%s][%s:%s:%d]->", Logger::logLevelToString(lv).c_str(), loggable::getLogTime().c_str(), __FILE__, __FUNCTION__, __LINE__);\
            fprintf(stream, fmt"\n", ##__VA_ARGS__); \
            if(Log::newInstance().isLogScreen()) \
            {\
                printf("[%s][%s][%s:%s:%d]->", Logger::logLevelToString(lv).c_str(), loggable::getLogTime().c_str(), __FILE__, __FUNCTION__, __LINE__);\
                printf(fmt"\n", ##__VA_ARGS__); \
            }\
        }\
    }\
}

#define LOG_COMMON_EASY(lv, fmt, ...) \
{\
    if (lv >= Log::newInstance().getLogLevel())\
    {\
        FILE *stream = Log::newInstance().getLogStream(Debug); \
        fprintf(stream, "[%s][%s][%s:%s:%d]->", Logger::logLevelToString(lv).c_str(), loggable::getLogTime().c_str(), __FILE__, __FUNCTION__, __LINE__);\
        fprintf(stream, fmt"\n", ##__VA_ARGS__); \
        if(Log::newInstance().isLogScreen()) \
        {\
            printf("[%s][%s][%s:%s:%d]->", Logger::logLevelToString(lv).c_str(), loggable::getLogTime().c_str(), __FILE__, __FUNCTION__, __LINE__);\
            printf(fmt"\n", ##__VA_ARGS__); \
        }\
    }\
}

#ifndef NO_THREAD_SAFE
#define LOG_DEBUG_EASY(fmt, ...) { LOG_COMMON_THREAD_SAFE_EASY(Debug, fmt, ##__VA_ARGS__) }
#define LOG_INFO_EASY(fmt, ...) { LOG_COMMON_THREAD_SAFE_EASY(Info, fmt, ##__VA_ARGS__) }
#define LOG_WARN_EASY(fmt, ...) { LOG_COMMON_THREAD_SAFE_EASY(Warn, fmt, ##__VA_ARGS__) }
#define LOG_ERROR_EASY(fmt, ...) { LOG_COMMON_THREAD_SAFE_EASY(Error, fmt, ##__VA_ARGS__) }
#define LOG_FATAL_EASY(fmt, ...) { LOG_COMMON_THREAD_SAFE_EASY(Fatal, fmt, ##__VA_ARGS__) }
#else
#define LOG_DEBUG_EASY(fmt, ...) { LOG_COMMON_EASY(Debug, fmt, ##__VA_ARGS__) }
#define LOG_INFO_EASY(fmt, ...) { LOG_COMMON_EASY(Info, fmt, ##__VA_ARGS__) }
#define LOG_WARN_EASY(fmt, ...) { LOG_COMMON_EASY(Warn, fmt, ##__VA_ARGS__) }
#define LOG_ERROR_EASY(fmt, ...) { LOG_COMMON_EASY(Error, fmt, ##__VA_ARGS__) }
#define LOG_FATAL_EASY(fmt, ...) { LOG_COMMON_EASY(Fatal, fmt, ##__VA_ARGS__) }
#endif
    }

}

#endif