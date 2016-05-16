#ifndef __H_STRING_UTILS__
#define __H_STRING_UTILS__

#include <string>
#include <stdint.h>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <map>

#ifdef _WIN32
#ifndef snprintf
#define snprintf _snprintf_s
#endif
#endif

namespace utils
{

    typedef std::string String;
    typedef std::vector<String> StringList;

    class StringUtils
    {
    public:
	    StringUtils();
	    ~StringUtils();

    public:
	    static StringList &split(const String &str, const String &cs, StringList &buf, uint32_t count = -1);
	    static StringList &rSplit(const String &str, const String &cs, StringList &buf, uint32_t count = -1);
	    static String &join(const StringList &list, const String &linker, String &buf);
	    static String &replace(String &str, const String &src, const String &dest, uint32_t count = -1);
	    static String &toLower(String &str);
	    static String &toUpper(String &str);
	    static String &capitalize(String &str);
	    static String &lTrim(String &str, const String &cs = " \t\n");
	    static String &rTrim(String &str, const String &cs = " \t\n");
	    static String &trim(String &str, const String &cs = " \t\n");
	    static String &just(String &str, const char c, const uint32_t &size);
	    static String &rJust(String &str, const char c, const uint32_t &size);
	    static bool isUpperCase(const String &str);
	    static bool isLowerCase(const String &str);
	    static bool isStartsWith(const String &str, const String &prefix);
	    static bool isEndsWith(const String &str, const String &postfix);
	    static bool isAlpha(const String &str);
	    static bool isDigit(const String &str);
	    static bool isAlnum(const String &str);
        static bool isSpace(const String &str);
	    static String &toHexString(String &str, const String &prefix = "\\x");
	    static String &fromHexString(String &str, const String &prefix = "\\x");
        static long toLong(const String &str, const uint32_t &radix = 10);
        static unsigned long toULong(const String &str, const uint32_t &radix = 10);
        static int toInt(const String &str, const uint32_t &radix = 10);
        static unsigned int toUInt(const String &str, const uint32_t &radix = 10);
        static double toDouble(const String &str);
        static String &wrap(String &str, const String &wrapStr);
        static String &wrap(String &str, const String &lWrap, const String &rWrap);

    public:
        static StringList split(const String &str, const String &cs, uint32_t count = -1);
        static StringList rSplit(const String &str, const String &cs, uint32_t count = -1);
        static String join(const StringList &list, const String &linker);
        static String replace(const String &str, const String &src, const String &dest, uint32_t count = -1);
        static String toLower(const String &str);
        static String toUpper(const String &str);
        static String capitalize(const String &str);
        static String lTrim(const String &str, const String &cs = " \t\n");
        static String rTrim(const String &str, const String &cs = " \t\n");
        static String trim(const String &str, const String &cs = " \t\n");
        static String just(const String &str, const char c, const uint32_t &size);
        static String rJust(const String &str, const char c, const uint32_t &size);
        static String toHexString(const String &str, const String &prefix = "\\x");
        static String fromHexString(const String &str, const String &prefix = "\\x");
        static String wrap(const String &str, const String &wrapStr);
        static String wrap(const String &str, const String &lWrap, const String &rWrap);

    protected:
        static String nullList() { return "[]"; }
        static String nullMap() { return "{}"; }

    public:
        static String toString(const int &value) { return digitalToString("%d", value); }
        static String toString(const unsigned int &value) { return digitalToString("%u", value); }
        static String toString(const long &value) { return digitalToString("%ld", value); }
        static String toString(const unsigned long &value) { return digitalToString("%lu", value); }
        static String toString(const double &value) { return digitalToString("%g", value); }
        static String toString(const float &value) { return digitalToString("%g", value); }
        static String toString(const String &str) { return str; }
        static String toString(const char *str) { return String(str); }
#if __cplusplus >=201103L
        static String toString(long long value) { return digitalToString("%lld", value); }
        static String toString(unsigned long long value) { return digitalToString("%llu", value); }
#endif

        template<typename Key, typename Value>
        static String &toString(const std::map<Key, Value> &m, String &buf)
        {
            typename std::map<Key, Value>::const_iterator it = m.begin();
            typename std::map<Key, Value>::const_iterator end = m.end();
            if (it == end)
            {
                buf.append(nullMap());
                return buf;
            }

            --end;
            buf.append("{");
            for (; it != end; ++it)
            {
                buf.append(StringUtils::toString(it->first));
                buf.append(":");
                buf.append(StringUtils::toString(it->second));
                buf.append(", ");
            }
            if (m.size() != 0)
            {
                buf.append(StringUtils::toString(it->first));
                buf.append(":");
                buf.append(StringUtils::toString(it->second));
            }
            buf.append("}");
            return buf;
        }
        template<typename Key, typename Value>
        static String toString(const std::map<Key, Value> &m)
        {
            String buf;
            return toString(m, buf);
        }

        template<typename Element>
        static String &toString(const Element &elem, String &buf)
        {
            typename Element::const_iterator end = elem.end();
            typename Element::const_iterator it = elem.begin();
            if (it == end)
            {
                buf.append(nullList());
                return buf;
            }

            --end;
            buf.append("[");
            for (; it != end; ++it)
            {
                buf.append(StringUtils::toString(*it));
                buf.append(", ");
            }
            if (elem.size() != 0)
                buf.append(StringUtils::toString(*it));
            buf.append("]");
            return buf;
        }
        template<typename Element>
        static String toString(const Element &elem)
        {
            String buf;
            return toString(elem, buf);
        }

        template<typename Type, int32_t n>
        static String toString(const Type (&elem)[n], String &buf)
        {
            buf.append("[");
            for (int i = 0; i < n; ++i)
            {
                buf.append(StringUtils::toString(elem[i]));
                if (i != n - 1)
                    buf.append(", ");
            }
            buf.append("]");
            return buf;
        }
        template<typename Type, int32_t n>
        static String toString(const Type(&elem)[n])
        {
            String buf;
            return toString(elem, buf);
        }

    private:
        template<typename Type>
        static String digitalToString(const char *fmt, const Type &val)
        {
            char buf[CONV_TO_STR_SIZE];
            uint32_t len = snprintf(buf, sizeof(buf), fmt, val);
            return String(buf, len);
        }

    private:
        template<typename Iterable, typename Func>
        static bool circleJudge(Iterable iter, Func judge)
        {
            for (typename Iterable::const_iterator it = iter.begin(); it != iter.end(); ++it)
            {
                if (!judge(*it))
                    return false;
            }
            return iter.empty() ? false : true;
        }

    private:
        static const uint32_t CONV_TO_STR_SIZE = 30;

    };
}

#endif
