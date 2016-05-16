#include "Json.h"

namespace parser
{
    String &IntValue::toString(String &buf) const
    {
        return buf.append(StringUtils::toString(intValue()));
    }
    
    String &FloatValue::toString(String &buf) const
    {
        return buf.append(StringUtils::toString(floatValue()));
    }

    String &NilValue::toString(String &buf) const
    {
        buf.append("null");
        return buf;
    }

    String &BoolValue::toString(String &buf) const
    {
        if (boolValue())
            buf.append("true");
        else
            buf.append("false");
        return buf;
    }
    String &StringValue::toString(String &buf) const
    {
        buf.append("\"");
        for (String::const_iterator it = stringValue().begin(); it != stringValue().end(); ++it)
        {
            switch (*it)
            {
            case '\\': buf += "\\\\"; break;
            case '\t': buf += "\\t"; break; 
            case '\b': buf += "\\b"; break;
            case '\v': buf += "\\v"; break;
            case '\f': buf += "\\f"; break;
            case '\n': buf += "\\n"; break; 
            case '"':  buf += "\\\""; break; 
            case '\a': buf += "\\a"; break;
            case '\r': buf += "\\r"; break;
            case '/': buf += "\\/"; break;
            default:
                buf += *it;
                break;
            }
        }
        buf.append("\"");
        return buf;
    }
    String &ArrayValue::toString(String &buf) const
    {
        buf.append("[");
        for (Array::const_iterator it = arrayValue().begin(); it != arrayValue().end(); ++it)
        {
            it->toString(buf);
            buf.append(",");
        }
        if (arrayValue().size() != 0)
            buf.erase(buf.end() - 1);
        buf.append("]");
        return buf;
    }
    String &ObjectValue::toString(String &buf) const
    {
        buf.append("{");
        for (Object::const_iterator it = objectValue().begin(); it != objectValue().end(); ++it)
        {
            buf.append(StringUtils::wrap(it->first, "\""));
            buf.append(":");
            it->second.toString(buf);
            buf.append(",");
        }
        if (objectValue().size() != 0)
            buf.erase(buf.end() - 1);
        buf.append("}");
        return buf;
    }

    /*
    static Json &nullJson()
    {
        static Json json;
        return json;
    }
    */

    Json JsonParser::parseFromFile(const String &file)
    {
        InStream in;
        in.open(file.c_str(), std::ios::in);
        if (!in.is_open())
            throw except::FileNotFoundException(String("Couldn't find file:").append(file));
        return parseFromStream(in);
        //in.close();
    }
    Json JsonParser::parseFromStream(InStream &in)
    {
        Reader reader(ReadableWrap(new StreamReader(in)));
        return parseFromReader(reader);
    }
    Json JsonParser::parseFromString(const String &str)
    {
        Reader reader(ReadableWrap(new StringReader(str)));
        return parseFromReader(reader);
    }

    Json JsonParser::parseFromReader(Reader &reader)
    {
        json_err_check(!reader.isEnd(), "Unexpect end when begin parse!!");
        reader.skip(skipSpace);
        char c = reader.top();
        if (c == '+' || c == '-' || (c >= '0' && c <= '9'))
            return parseJsonNumber(reader);
        else if (c == '"')
            return parseJsonString(reader);
        else if (c == 't')
            return acquire(reader, "true", true);
        else if (c == 'f')
            return acquire(reader, "false", false);
        else if (c == 'n')
            return acquire(reader, "null", Nil());
        else if (c == '{')
            return parseJsonObject(reader);
        else if (c == '[')
            return parseJsonArray(reader);
        json_err_check(false, "Unknow character:" + c);
    }

    Json JsonParser::parseJsonObject(Reader &reader)
    {
        Object obj;
        char c;
        reader.skip(1);
        while (1)
        {
            reader.skip(skipSpace);
            c = reader.top();
            if (c == '}')
            {
                reader.skip(1);
                break;
            }
            json_err_check(c == '"', "Need '\"', but got a '" + c + "'");
            String key;
            parseJsonString(reader, key);
            c = reader.readChar(skipSpace);
            json_err_check(c == ':', "Need ':', but got a '" + c + "'");
            obj[key] = parseFromReader(reader);
            if ((c = reader.readChar(skipSpace)) == '}')
                break;
            json_err_check(c == ',', "Need ',', but got a '" + c + "'");
        }
        return Json(obj);
    }

    Json JsonParser::parseJsonArray(Reader &reader)
    {
        reader.skip(1);
        Array array;
        char c;

        while (1)
        {
            if (reader.top() == ']')
            {
                reader.skip(1);
                break;
            }
            array.push_back(parseFromReader(reader));
            if ((c = reader.readChar(skipSpace)) == ']')
                break;
            json_err_check(c == ',', "Need ',', but got a '" + c + "'");
        }
        return Json(array);
    }

    bool JsonParser::writeJsonToFile(const Json &json, const String &file)
    {
        OutStream out;
        out.open(file.c_str(), std::ios::out);
        if (!out.is_open())
            return false;
        bool ret = writeJsonToStream(json, out);
        out.close();
        return ret;
    }

    bool JsonParser::writeJsonToStream(const Json &json, OutStream &out)
    {
        String buf;
        if (json.getTag() != ARRAY && json.getTag() != OBJECT)
            return false;
        out << writeJsonToString(json, buf);
        out.close();
        return true;
    }

    String &JsonParser::writeJsonToString(const Json &json, String &buf)
    {
        return json.toString(buf);
    }

    String JsonParser::writeJsonToString(const Json &json)
    {
        String buf;
        return writeJsonToString(json, buf);
    }

    Json JsonParser::acquire(Reader &reader, const String &str, Json ret)
    {
        uint32_t count = 0;
        while (!reader.isEnd() && count != str.size())
        {
            if (str.at(count) != reader.readChar())
                break;
            ++count;
        }
        if (count == str.size())
            return ret;
        return Json();
    }

    Json JsonParser::parseJsonNumber(Reader &reader)
    {
        /* PARSER FROM:http://www.json.org/ */
        String buf;
        char c;
        json_err_check(!reader.isEnd(), "Unexpect end while parsing json number");
        //parse integer
        if (reader.top() == '+' || reader.top() == '-')
            buf += reader.readChar();
        json_err_check(!reader.isEnd(), "Unexpect end while parsing json number");
        json_err_check(isdigit(reader.top()), "Need a digit but got a '" + reader.top() + "'");
        if (reader.top() == '0')
            buf += reader.readChar();
        else
        {
            while (!reader.isEnd() && isdigit(c = reader.top()))
                buf += reader.readChar();
        }
        //parse decimal
        do
        {
            if (!reader.isEnd() && reader.top() == '.')
            {
                buf += reader.readChar();
                json_err_check(!reader.isEnd(), "Unexpect end, need a '.'");
                json_err_check(isdigit(reader.top()), "Need a digit but got a '" + reader.top() + "'");
                while (!reader.isEnd() && isdigit(c = reader.top()))
                    buf += reader.readChar();
            }
            else
                break;
        } while (0);
        //parse e..
        do
        {
            if (!reader.isEnd() && tolower(reader.top()) == 'e')
            {
                buf += reader.readChar();
                json_err_check(!reader.isEnd(), "Unexpect end, need some digit or '+' or '-' after e");
                if (reader.top() == '-' || reader.top() == '+')
                    buf += reader.readChar();
                json_err_check(!reader.isEnd(), "Unexpect end, need some digit after e");
                json_err_check(isdigit(reader.top()), "Need a digit but got a '" + reader.top() + "'");
                while (!reader.isEnd() && isdigit(c = reader.top()))
                    buf += reader.readChar();
            }
            else
                break;
        } while (0);
        return Json(StringUtils::toDouble(buf));
    }

    Json JsonParser::parseJsonString(Reader &reader)
    {
        String buf;
        parseJsonString(reader, buf);
        return Json(buf);
    }

    String &JsonParser::parseJsonString(Reader &reader, String &buf)
    {
        buf.clear();
        reader.skip(1);
        json_err_check(!reader.isEnd(), "Unexpect end when begin parse string!");
        char c;

        while (!reader.isEnd() && (c = reader.top()) != '"')
        {

            if (c == '\\')
            {
                json_err_check(!reader.isEnd(), "Unexpect end, need a character after '\\'");
                reader.skip(1);
                c = reader.readChar();
                switch (c)
                {
                case 'b':
                    buf.push_back('\b'); 
                    break;
                case 'n':
                    buf.push_back('\n'); 
                    break;
                case 'a':
                    buf.push_back('\a'); 
                    break;
                case 'v':
                    buf.push_back('\v'); 
                    break;
                case 'f':
                    buf.push_back('\f');
                    break;
                case '"':
                    buf.push_back('"'); 
                    break;
                case 'r':
                    buf.push_back('\r'); 
                    break;
                case 't':
                    buf.push_back('\t'); 
                    break;
                case '/':
                case '\\':
                    buf.push_back(c); 
                    break;
                case 'u':
                {
                    int i;
                    String tmp("0x");
                    for (i = 0; i < 4; ++i)
                    {
                        json_err_check(!reader.isEnd(), "Unexpect end, need 4 digit to decode!");
                        tmp += reader.readChar();
                    }
                    long code = StringUtils::toLong(tmp, 16);
                    /* FROM:https://github.com/kbenzie/json/blob/master/source/json.cpp */
                    if(code >= 0x0800 && code <= 0xFFFF) 
                    {
                        // NOTE: Write three byte UTF-8 code point
                        buf.push_back(0xe0 | ((code >> 12) & 0xf));
                        buf.push_back(0x80 | ((code >> 6) & 0x3f));
                        buf.push_back(0x80 | (code & 0x3f));
                    }
                    else if(code >= 0x0080 && code <= 0x07FF) 
                    {
                        buf.push_back(0xc0 | ((code >> 6) & 0x3f));
                        buf.push_back(0x80 | (code & 0x3f));
                    }
                    else if(code >= 0x0000 && code <= 0x007F) 
                    {
                        // NOTE: Write single byte UTF-8 code point
                        buf.push_back(0x00 | (code & 0x7f));
                    }
                    break;
                }
                default:
                    json_err_check(false, "Unknow character '" + c + "' after '\\'!");
                }
            }
            else
            {
                c = reader.readChar();
                buf.push_back(c);
            }
        }
        c = reader.readChar();
        json_err_check(c == '"', "Need '\"', but got a '" + c + "'");
        return buf;
    }

    String &Json::toString(String &buf) const
    {
        return getValue().toString(buf);
    }


    Json::Json() { value_.reset(new NilValue(Nil())); }
    Json::Json(const int32_t &value) { value_.reset(new IntValue(value)); }
    Json::Json(const double &value) { value_.reset(new FloatValue(value)); }
    Json::Json(const bool value) { value_.reset(new BoolValue(value)); }
    Json::Json(const String &value) { value_.reset(new StringValue(value)); }
    Json::Json(const Nil &value) { value_.reset(new NilValue(value)); }
    Json::Json(const char *value) { value_.reset(new StringValue(value)); }
    Json::Json(const Array &value) { value_.reset(new ArrayValue(value)); }
    Json::Json(const Object &value) { value_.reset(new ObjectValue(value)); }

    Json &Json::operator=(const Json &json) 
    { 
        value_ = const_cast<ValueWrap &>(json.value_); return *this; 
    }

    bool Json::operator==(const Json &json) const
    { 
        return  this->equals(json);
    }

    bool Json::operator!=(const Json &json) const
    {
        return !(*this == json);
    }

    bool Json::equals(const Json &json) const 
    { 
        return getValue().getTag() == json.getValue().getTag() && getValue() == json.getValue(); 
    }

    JsonTag Json::getTag() const 
    { 
        return getValue().getTag();
    }

    bool Json::isInteger() const 
    {
        return getValue().isInteger();
    }

    bool Json::isFloat() const 
    { 
        return getValue().isFloat();
    }

    bool Json::isNil() const
    { 
        return getValue().isNil();
    }

    bool Json::isArray() const 
    { 
        return getValue().isArray();
    }

    bool Json::isBool() const 
    { 
        return getValue().isBool();
    }

    bool Json::isString() const 
    { 
        return getValue().isString();
    }

    bool Json::isObject() const 
    { 
        return getValue().isObject(); 
    }

    int32_t Json::intValue() const 
    { 
        return getValue().intValue();
    }

    double Json::floatValue() const
    { 
        return getValue().floatValue();
    }

    bool Json::boolValue() const
    { 
        return getValue().boolValue(); 
    }

    const String &Json::stringValue() const
    {
        return getValue().stringValue();
    }

    const Object &Json::objectValue() const
    {
        return getValue().objectValue(); 
    }

    const Array &Json::arrayValue() const
    { 
        return getValue().arrayValue(); 
    }

    String &Json::stringValue() 
    {
        return getValue().stringValue(); 
    }

    Object &Json::objectValue() 
    { 
        return getValue().objectValue(); 
    }

    Array &Json::arrayValue() 
    { 
        return getValue().arrayValue();
    }

    const Json &Json::operator[](size_t index) const
    { 
        json_err_checkNoReader(getTag() == ARRAY, "Use [] to call a non-array json object!");
        if (index > getValue().arrayValue().size())
            throw except::OutOfRangeException();
        return getValue().arrayValue().at(index);
    }

    Json &Json::operator[](size_t index)
    { 
        json_err_checkNoReader(getTag() == ARRAY, "Use [] to call a non-array json object!");
        return getValue().arrayValue()[index];
    }

    const Json &Json::operator[](const String &key) const
    {
        json_err_checkNoReader(getTag() == OBJECT, "Use [] to call a non-object json object!");
        if (getValue().objectValue().count(key) == 0)
            throw except::KeyNotFoundException();
        return getValue().objectValue().at(key);
    }

    Json &Json::operator[](const String &key)
    {
        json_err_checkNoReader(getTag() == OBJECT, "Use [] to call a non-object json object!");
        return getValue().objectValue()[key];
    }

    String Json::toString() const 
    {
        String buf; 
        return toString(buf); 
    }

    const Value &Json::getValue() const 
    { 
        return *value_;
    }

    Value &Json::getValue()
    { 
        return *value_; 
    }
}
