#include "Logger.h"

namespace loggable
{
    namespace debugger
    {
        thread::mutex::Mutex &getMutex()
        {
            static thread::mutex::Mutex mu;
            return mu;
        }
    }

    namespace easy_log
    {
        Logger::~Logger()
        {
            releaseLogMap();
        }

        void Logger::init(const String & file)
        {
#ifndef NO_THREAD_SAFE
            ScopeLock<Mutex> lock(getMutex());
#endif
            if (isInited())
                throw except::MultiSetException("Couldn't init twice!");
            parser::Json js = parser::JsonParser::parseFromFile(file);
            setLogLevel(Debug);
            setLogScreen(true);
            for (parser::Object::const_iterator it = js.objectValue().begin(); it != js.objectValue().end(); ++it)
            {
                String name = StringUtils::toLower(it->first);
                if (name == "loglevel")
                    setLogLevel(static_cast<LogLevel>(it->second.intValue()));
                else if (name == "debug")
                    setLogStream(Debug, it->second.stringValue());
                else if (name == "info")
                    setLogStream(Info, it->second.stringValue());
                else if (name == "warn")
                    setLogStream(Warn, it->second.stringValue());
                else if (name == "error")
                    setLogStream(Error, it->second.stringValue());
                else if (name == "fatal")
                    setLogStream(Fatal, it->second.stringValue());
                else if (name == "logscreen")
                    setLogScreen(it->second.boolValue());
                else
                    throw except::UnKnowKeyException(String("Unknow key:") + it->first);
            }
            setInit(true);
        }

        FILE * Logger::getLogStream(LogLevel lv)
        {
            if (getLogMap().find(lv) == getLogMap().end())
                throw except::KeyNotFoundException(String("Couldn't find stream refer to level:").append(logLevelToString(lv)));
            return getLogMap().find(lv)->second;
        }

//        void Logger::append(LogLevel lv, const char *file, const char *func, const uint32_t &line, const char *fmt, const char *msg)
//        {
//#ifndef NO_THREAD_SAFE
//            ScopeLock<Mutex> lock(getMutex());
//#endif
//            LogMap::iterator it = getLogMap().find(lv);
//            if (it == getLogMap().end())
//                throw except::FileNotFoundException(String("Couldn't find stream refer to level:").append(logLevelToString(lv)));
//            fprintf(it->second, "[%s][%s][%s:%s:%u]->", logLevelToString(lv).c_str(), file, func, line);
//            fprintf(it->second, fmt, msg);
//
//        }

        const String & Logger::logLevelToString(LogLevel lv)
        {
            const static String lvArray[] = { "Debug", "Info", "Warn", "Error", "Fatal" };
            return lvArray[(int)lv];
        }

        void Logger::setLogStream(LogLevel lv, const String &file)
        {
            if (getLogMap().find(lv) != getLogMap().end())
                throw except::MultiSetException(String("Couldn't set log stream twice, you have a same tag in your log setting? LogLevel:").append(logLevelToString(lv)));
            FILE *fp = fopen(file.c_str(), "a+");
            if (fp == NULL)
            {
                releaseLogMap();
                throw except::FileNotFoundException(String("Couldn't open file:").append(file));
            }
            getLogMap()[lv] = fp;
        }
        void Logger::releaseLogMap()
        {
            for (LogMap::iterator it = getLogMap().begin(); it != getLogMap().end(); ++it)
                fclose(it->second);
            getLogMap().clear();
        }
    }

    utils::String getLogTime()
    {
#ifdef _WIN32
        char buf[30];
        SYSTEMTIME st;
        GetLocalTime(&st);
        struct tm gm = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1, st.wYear - 1900, st.wDayOfWeek, 0, 0 };
        size_t sz = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &gm);
        //snprintf(buf + sz, sizeof(buf) - sz, "%u", st.wMilliseconds);
        return utils::String(buf).append(".").append(utils::StringUtils::toString((uint32_t)st.wMilliseconds));
#else
        char buf[30];
        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm gm;
        localtime_r(&tv.tv_sec, &gm);
        size_t sz = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &gm);
        return utils::String(buf).append(".").append(utils::StringUtils::toString((uint32_t)tv.tv_usec / 1000));
#endif
    }

}
