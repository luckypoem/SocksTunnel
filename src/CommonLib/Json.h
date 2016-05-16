#ifndef __H_JSON__
#define __H_JSON__

#if __cplusplus >= 201103L
#include <memory>
#else
#include "SharedPtr.h"
#endif
#include <vector>
#include <map>
#include <string>
#include "StringUtils.h"
#include <fstream>
#include <assert.h>
#include "Reader.h"
#include "Exception/ParseException.h"
#include "Exception/KeyNotFoundException.h"
#include "Exception/OutOfRangeException.h"
#include "Exception/FileNotFoundException.h"

namespace parser
{
    using utils::StringUtils;
    using io::Reader;
    using io::StreamReader;
    using io::ReadableWrap;
    using io::StringReader;
    enum JsonTag { NIL, STRING, BOOL, INTEGER, FLOAT, OBJECT, ARRAY};
    typedef std::ifstream InStream;
    typedef std::ofstream OutStream;

    class Nil;
    class NilValue;
    class Json;
    class Value;
    class IntValue;
    class FloatValue;
    class NulValue;
    class BoolValue;
    class StringValue;
    class ObjectValue;
    class ArrayValue;
    class BasicRet;

    typedef std::string String;
    typedef std::vector<Json> Array;

    typedef ptr::SharedPtr<Value> ValueWrap;
    typedef ptr::SharedPtr<String> StringWrap;
    typedef std::map<String, Json> Object;

#define json_err_check(express, msg) \
{   \
    if(!(express))\
        throw except::ParseException(String("FILE:") + StringUtils::toString(__FILE__) + " FUNC:" + __FUNCTION__ + " LINE:" + StringUtils::toString(__LINE__) + " -> " + msg + " index at:" + StringUtils::toString(reader.current()));\
}
#define json_err_checkNoReader(express, msg) \
{\
    if(!(express))\
        throw except::ParseException(String("FILE:") + StringUtils::toString(__FILE__) + " FUNC:" + __FUNCTION__ + " LINE:" + StringUtils::toString(__LINE__) + " -> " + msg);\
}

    class Json
    {
    public:
        Json();
        Json(const int32_t &value);
        Json(const double &value);
        Json(const bool value);
        Json(const String &value);
        Json(const Nil &value);
        Json(const char *value);
        Json(const Array &value);
        Json(const Object &value);

    public:
        Json &operator=(const Json &json);
        bool operator==(const Json &json) const;
        bool operator!=(const Json &json) const;
        bool equals(const Json &json) const;

    public:
        JsonTag getTag() const;
        bool isInteger() const;
        bool isFloat() const;
        bool isNil() const;
        bool isArray() const;
        bool isBool() const;
        bool isString() const;
        bool isObject() const;

    public:
        int32_t intValue() const;
        double floatValue() const;
        bool boolValue() const;

        const String &stringValue() const;
        const Object &objectValue() const;
        const Array &arrayValue() const;
        String &stringValue();
        Object &objectValue();
        Array &arrayValue();

        const Json &operator[](size_t index) const;
        Json &operator[](size_t index);
        const Json &operator[](const String &key) const;
        Json &operator[](const String &key);

    public:
        String &toString(String &buf) const;
        String toString() const;

    protected:
        const Value &getValue() const;
        Value &getValue();

    private:
        ValueWrap value_;
    };

    class Nil 
    {
    public:
        Nil() : value_(false) {}
        ~Nil() {}
        bool operator==(const Nil &value) const { return value_ == value.value_; }
        operator int() { return 0; }

    private:
        bool value_;
    };

    class BasicRet
    {
    private:
        String basicString_;
        Array basicArray_;
        Object basicObject_;
        int32_t basicInt_;
        double basicFloat_;
        bool basicBool_;
        Nil basicNil_;
        static BasicRet &item()
        {
            static BasicRet basic;
            return basic;
        }

    public:
        static String &getBasicString() { return item().basicString_; }
        static Array &getBasicArray() { return item().basicArray_; }
        static Object &getBasicObject() { return item().basicObject_; }
        static int32_t &getBasicInteger() { return item().basicInt_; }
        static double &getBasicFloat() { return item().basicFloat_; }
        static bool &getBasicBool() { return item().basicBool_; }
        static Nil &getBasicNil() { return item().basicNil_; }
    };

    class Value
    {
    public:
        Value() {}
        virtual JsonTag getTag() const = 0;
        virtual int32_t intValue() const { return BasicRet::getBasicInteger(); }
        virtual double floatValue() const { return BasicRet::getBasicFloat(); }
        virtual bool boolValue() const { return BasicRet::getBasicBool(); }
        virtual Nil nilValue() const { return BasicRet::getBasicNil(); }
        virtual const String &stringValue() const { return BasicRet::getBasicString(); }
        virtual String &stringValue() { return BasicRet::getBasicString(); }
        virtual Object &objectValue() { return BasicRet::getBasicObject(); }
        virtual const Object &objectValue() const { return BasicRet::getBasicObject(); }
        virtual Array &arrayValue() { return BasicRet::getBasicArray(); }
        virtual const Array &arrayValue() const { return BasicRet::getBasicArray(); }

        bool operator==(const Value &value) const { return this->equals(value); }
        virtual bool equals(const Value &value) const = 0;
        bool operator!=(const Value &value) const { return !(*this == value); }

        virtual bool isInteger() const { return false; }
        virtual bool isFloat() const { return false; }
        virtual bool isNil() const { return false; }
        virtual bool isArray() const { return false; }
        virtual bool isBool() const { return false; }
        virtual bool isString() const { return false; }
        virtual bool isObject() const { return false; }

        String toString() { String buf; return toString(buf); }
        virtual String &toString(String &buf) const = 0;

        virtual ~Value() {};
    };

    template<class Type, JsonTag tag>
    class ValueTemp : public Value
    {
    public:
        ValueTemp() {}
        ~ValueTemp() {}

    public:
        JsonTag getTag() const { return tag; };
        bool isInteger() const { return getTag() == INTEGER; }
        bool isFloat() const { return getTag() == FLOAT; }
        bool isNil() const { return getTag() == NIL; }
        bool isArray() const { return getTag() == ARRAY; }
        bool isBool() const { return getTag() == BOOL; }
        bool isString() const { return getTag() == STRING; }
        bool isObject() const { return getTag() == OBJECT; }

        bool equals(const Value &value) const { return type_ == dynamic_cast<const ValueTemp<Type, tag> &>(value).type_; }
    protected:
        Type type_;
    };

    class IntValue : public ValueTemp<int32_t, INTEGER>
    {
    public:
        explicit IntValue() { static_cast<int32_t &>(type_) = 0; }
        explicit IntValue(const int32_t &value) { type_ = value; }
        explicit IntValue(const IntValue &value) { type_ = value.intValue(); }

    public:
        int32_t intValue() const { return type_; }
        bool equals(const Value &value) const { return intValue() == value.intValue(); }

        String &toString(String &buf) const;
    };

    class FloatValue : public ValueTemp<double, FLOAT>
    {
    public:
        explicit FloatValue() { type_ = 0; }
        explicit FloatValue(const double &value) { type_ = value; }
        explicit FloatValue(const FloatValue &value) { type_ = value.intValue(); }

    public:
        double floatValue() const { return type_; }
        int32_t intValue() const { return static_cast<int32_t>(type_); }
        bool equals(const Value &value) const { return floatValue() == value.floatValue(); }
        String &toString(String &buf) const;
    };

    class NilValue : public ValueTemp<Nil, NIL> 
    {
    public:
        explicit NilValue() { type_ = Nil(); }
        explicit NilValue(const Nil &value) { type_ = value; }
        explicit NilValue(const NilValue &value) { type_ = value.type_; }

    public:
        Nil nilValue() const { return type_; }
        bool equals(const Value &value) const { return nilValue() == value.nilValue(); }
        String &toString(String &buf) const;
    };

    class BoolValue : public ValueTemp<bool, BOOL>
    {
    public:
        explicit BoolValue() { type_ = false; }
        explicit BoolValue(const bool value) { type_ = value; }
        explicit BoolValue(const BoolValue &value) { type_ = value.boolValue(); }

    public:
        bool boolValue() const { return type_; }
        bool equals(const Value &value) const { return boolValue() == value.boolValue(); }
        String &toString(String &buf) const;
    };

    class StringValue : public ValueTemp<String, STRING>
    {
    public:
        StringValue() { type_ = String(); }
        explicit StringValue(const String &value) { type_ = value; }
        explicit StringValue(const char *value) { type_ = value; }
        explicit StringValue(const StringValue &value) { type_ = value.stringValue(); }

    public:
        const String &stringValue() const { return type_; }
        String &stringValue() { return type_; }
        bool equals(const Value &value) const { return stringValue() == value.stringValue(); }
        String &toString(String &buf) const;
    };

    class ArrayValue : public ValueTemp<Array, ARRAY>
    {
    public:
        explicit ArrayValue() { type_ = Array(); }
        explicit ArrayValue(const Array &value) { type_ = value; }
        explicit ArrayValue(const ArrayValue &value) { type_ = value.arrayValue(); }

    public:
        const Array &arrayValue() const { return type_; }
        Array &arrayValue() { return type_; }
        String &toString(String &buf) const;
    };

    class ObjectValue : public ValueTemp<Object, OBJECT>
    {
    public:
        explicit ObjectValue() { type_ = Object(); }
        explicit ObjectValue(const Object &value) { type_ = value; }
        explicit ObjectValue(const ObjectValue &value) { type_ = value.objectValue(); }

    public:
        const Object &objectValue() const { return type_; }
        Object &objectValue() { return type_; }
        String &toString(String &buf) const;
    };

    class JsonParser
    {
    public:
        static Json parseFromFile(const String &file);
        static Json parseFromStream(InStream &in);
        static Json parseFromString(const String &str);

    protected:
        static Json parseFromReader(Reader &reader);

    public:
        static bool writeJsonToFile(const Json &json, const String &file);
        static bool writeJsonToStream(const Json &json, OutStream &out);
        static String &writeJsonToString(const Json &json, String &buf);
        static String writeJsonToString(const Json &json);

    protected:
        static Json acquire(Reader &reader, const String &str, Json ret);
        static Json parseJsonNumber(Reader &reader);
        static String &parseJsonString(Reader &reader, String &buf);
        static Json parseJsonString(Reader &reader);
        static Json parseJsonObject(Reader &reader);
        static Json parseJsonArray(Reader &reader);

    private:
        JsonParser() {}
        JsonParser(const JsonParser &jp) {}
    };

    static bool skipSpace(char c)
    {
        return isspace(c) != 0;
    }
}

#endif
