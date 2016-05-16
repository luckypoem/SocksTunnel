#include "StringUtils.h"

namespace utils
{

    StringUtils::StringUtils()
    {
    }


    StringUtils::~StringUtils()
    {
    }

    StringList &StringUtils::split(const String &str, const String &cs, StringList &buf, uint32_t count)
    {
        String::size_type st, prev;
        buf.clear();
        //===special===
        if (cs.empty())
            return buf;
        if (str.empty())
        {
            buf.push_back("");
            return buf;
        }
        if (count == 0)
        {
            buf.push_back(str);
            return buf;
        }
        //===special===
        for (prev = 0, st = 0; (st = str.find(cs, st)) != String::npos && count > 0; --count)
        {
            buf.push_back(str.substr(prev, st - prev));
            st += cs.size();
            prev = st;
        }
        if(prev != str.size())
            buf.push_back(str.substr(prev));
        else
            buf.push_back("");
        return buf;
    }

    StringList &StringUtils::rSplit(const String &str, const String &cs, StringList &buf, uint32_t count)
    {
        StringUtils::split(str, cs, buf);
        uint32_t size = buf.size() - std::min(static_cast<uint32_t>(buf.size()), count);

        if(size != 0)
        {
            StringList front, tmp;
            String strBuf;
            front.assign(buf.begin(), buf.begin() + size);
            tmp.push_back(StringUtils::join(front, cs, strBuf));
            std::copy(buf.begin() + size, buf.end(), std::back_inserter(tmp));
            tmp.swap(buf);
        }
        return buf;
    }

    String &StringUtils::join(const StringList &list, const String &linker, String &buf)
    {
        buf.clear();
        if (list.empty())
            return buf;
        for (StringList::const_iterator it = list.begin(); it != list.end() - 1; ++it)
        {
            buf.append(*it);
            buf.append(linker);
        }
        buf.append(list.back());
        return buf;
    }

    String &StringUtils::replace(String &str, const String &src, const String &dest, uint32_t count)
    {
        String::size_type st, prev;
        String ret;
        for (prev = 0, st = 0; !str.empty() && (st = str.find(src, st)) != String::npos && count > 0; --count)
        {
            ret.append(str.substr(prev, st - prev));
            ret.append(dest);
            st += src.size();
            prev = st;
        }
        if (prev != str.size())
            ret.append(str.substr(prev));
        ret.swap(str);
        return str;
    }

    String &StringUtils::toLower(String &str)
    {
        for (String::iterator it = str.begin(); it != str.end(); ++it)
            *it = ::tolower(*it);
        return str;
    }

    String &StringUtils::toUpper(String &str)
    {
        for (String::iterator it = str.begin(); it != str.end(); ++it)
            *it = ::toupper(*it);
        return str;
    }

    String &StringUtils::capitalize(String &str)
    {
        if (str.size() < 1)
            return str;
        if (::isalpha(str.at(0)))
            str[0] = ::toupper(str.at(0));
        return str;
    }

    String &StringUtils::lTrim(String &str, const String &cs)
    {
        String::size_type st = str.find_first_not_of(cs);
        st = std::min(st, str.size());
        String tmp = str.substr(st);
        str.swap(tmp);
        return str;
    }

    String &StringUtils::rTrim(String &str, const String &cs)
    {
        String::size_type st = str.find_last_not_of(cs) + 1;
        st = std::min(st, str.size());
        String tmp = str.substr(0, st);
        str.swap(tmp);
        return str;
    }

    String &StringUtils::trim(String &str, const String &cs)
    {
        return lTrim(rTrim(str, cs), cs);
    }

    String &StringUtils::just(String &str, const char c, const uint32_t &size)
    {
        if (str.size() >= size)
            return str;
        unsigned int count = size - str.size();
        String ret(count, c);
        ret.append(str).swap(str);
        return str;
    }

    String &StringUtils::rJust(String &str, const char c, const uint32_t &size)
    {
        if (str.size() >= size)
            return str;
        for (unsigned int count = size - str.size(); count > 0; --count)
            str.push_back(c);
        return str;
    }

    bool StringUtils::isUpperCase(const String &str)
    {
        return circleJudge(str, ::isupper);
    }

    bool StringUtils::isLowerCase(const String &str)
    {
        return circleJudge(str, ::islower);
    }

    bool StringUtils::isStartsWith(const String &str, const String &prefix)
    {
	    return str.find(prefix) == 0;
    }

    bool StringUtils::isEndsWith(const String &str, const String &postfix)
    {
        String::size_type st = str.rfind(postfix);
	    return st + postfix.size() == str.size();
    }

    bool StringUtils::isAlpha(const String &str)
    {
        return circleJudge(str, ::isalpha);
    }

    bool StringUtils::isDigit(const String &str)
    {
        return circleJudge(str, ::isdigit);
    }

    bool StringUtils::isAlnum(const String &str)
    {
        return circleJudge(str, ::isalnum);
    }

    bool StringUtils::isSpace(const String & str)
    {
        return circleJudge(str, ::isspace);
    }

    String &StringUtils::toHexString(String &str, const String &prefix)
    {
        String ret;
        char buf[5];
        for (String::const_iterator it = str.begin(); it != str.end(); ++it)
        {
            snprintf(buf, sizeof(buf), "%x", *it);
            ret.append(prefix);
            ret.append(buf);
        }
        ret.swap(str);
        return str;
    }

    String &StringUtils::fromHexString(String &str, const String &prefix)
    {
        StringList list;
        char buf[5];
        StringUtils::split(str, prefix, list);
        str.clear();
        if(list.size() != 0)
        {
            for (StringList::const_iterator it = list.begin() + 1; it != list.end(); ++it)
            {
                snprintf(buf, sizeof(buf), "%c", static_cast<char>(StringUtils::toInt(*it, 16)));
                str.append(buf);
            }
        }
        return str;
    }

    long StringUtils::toLong(const String &str, const uint32_t &radix)
    {
        return strtol(str.c_str(), NULL, radix);
    }

    unsigned long StringUtils::toULong(const String &str, const uint32_t &radix)
    {
        return strtoul(str.c_str(), NULL, radix);
    }

    int StringUtils::toInt(const String &str, const uint32_t &radix)
    {
        return static_cast<int>(StringUtils::toLong(str.c_str(), radix));
    }

    unsigned int StringUtils::toUInt(const String &str, const uint32_t &radix)
    {
        return static_cast<unsigned int>(StringUtils::toInt(str.c_str(), radix));
    }

    double StringUtils::toDouble(const String &str)
    {
        return strtod(str.c_str(), NULL);
    }

    String &StringUtils::wrap(String &str, const String &wrapStr)
    {
        return wrap(str, wrapStr, wrapStr);
    }

    String &StringUtils::wrap(String &str, const String &lWrap, const String &rWrap)
    {
        String buf;
        buf.append(lWrap);
        buf.append(str);
        buf.append(rWrap);
        buf.swap(str);
        return str;
    }

    StringList StringUtils::split(const String &str, const String &cs, uint32_t count)
    {
        StringList list;
        return split(str, cs, list, count);
    }

    StringList StringUtils::rSplit(const String &str, const String &cs, uint32_t count)
    {
        StringList list;
        return rSplit(str, cs, list, count);
    }

    String StringUtils::join(const StringList &list, const String &linker)
    {
        String buf;
        return join(list, linker, buf);
    }

    String StringUtils::replace(const String &str, const String &src, const String &dest, uint32_t count)
    {
        String ret = str;
        return replace(ret, src, dest, count);
    }

    String StringUtils::toLower(const String &str)
    {
        String ret = str;
        return toLower(ret);
    }

    String StringUtils::toUpper(const String &str)
    {
        String ret = str;
        return toUpper(ret);
    }

    String StringUtils::capitalize(const String &str)
    {
        String ret = str;
        return capitalize(ret);
    }

    String StringUtils::lTrim(const String &str, const String &cs)
    {
        String ret = str;
        return lTrim(ret, cs);
    }

    String StringUtils::rTrim(const String &str, const String &cs)
    {
        String ret = str;
        return rTrim(ret, cs);
    }

    String StringUtils::trim(const String &str, const String &cs)
    {
        String ret = str;
        return trim(ret, cs);
    }

    String StringUtils::just(const String &str, const char c, const uint32_t &size)
    {
        String ret = str;
        return just(ret, c, size);
    }

    String StringUtils::rJust(const String &str, const char c, const uint32_t &size)
    {
        String ret = str;
        return rJust(ret, c, size);
    }

    String StringUtils::toHexString(const String &str, const String &prefix)
    {
        String ret = str;
        return toHexString(ret, prefix);
    }

    String StringUtils::fromHexString(const String &str, const String &prefix)
    {
        String ret = str;
        return fromHexString(ret, prefix);
    }

    String StringUtils::wrap(const String &str, const String &wrapStr)
    {
        String ret = str;
        return wrap(ret, wrapStr);
    }

    String StringUtils::wrap(const String &str, const String &lWrap, const String &rWrap)
    {
        String ret = str;
        return wrap(ret, lWrap, rWrap);
    }
}
