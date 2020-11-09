#pragma once

#include <libutils/String.h>

namespace json
{

struct Object;
struct Array;

enum Type
{
    ERROR = -1,

    STRING,
    INTEGER,
    DOUBLE,
    OBJECT,
    ARRAY,
    TRUE,
    FALSE,
    NIL,

    __TYPE_COUNT,
};

class Value
{
private:
    Type _type;

    union {
        StringStorage *_string;
        int _integer;
        double _double;
        Object *_object;
        Array *_array;

        uint64_t _all;

        static_assert(sizeof(uint64_t) >= sizeof(void *));
        static_assert(sizeof(uint64_t) >= sizeof(double));
    };

public:
    bool is(Type type) const { return _type == type; }

    String as_string() const;

    int as_integer() const;

    double as_double() const;

    const Object &as_object() const;

    const Array &as_array() const;

    Value();

    Value(String &);

    Value(String &&value);

    Value(const char *);

    Value(int);

    Value(double);

    Value(const Object &);

    Value(Object &&);

    Value(const Array &);

    Value(Array &&);

    Value(bool);

    Value(nullptr_t);

    Value(const Value &);

    Value(Value &&);

    ~Value();

    bool has(String key) const;

    const Value &get(String key) const;

    void put(String key, const Value &value);

    void remove(String key);

    size_t length() const;

    const Value &get(size_t index) const;

    void put(size_t index, const Value &value);

    void remove(size_t index);

    void append(const Value &value);

    Value &operator=(const Value &value);

    Value &operator=(Value &&value);

    void clear();
};

} // namespace json
