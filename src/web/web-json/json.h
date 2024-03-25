#pragma once

#include <karm-base/map.h>
#include <karm-base/string.h>
#include <karm-base/union.h>
#include <karm-base/vec.h>
#include <karm-io/emit.h>
#include <karm-io/expr.h>
#include <karm-io/fmt.h>
#include <karm-io/sscan.h>

namespace Web::Json {

struct Value;

using Array = Vec<Value>;

using Object = Map<String, Value>;

using Integer = isize;

#ifndef __ck_freestanding__
using Number = f64;
#endif

using _Store = Union<
    None,
    Array,
    Object,
    String,
    Integer,
#ifndef __ck_freestanding__
    Number,
#endif
    bool>;

struct Value {
    _Store _store;

    Value()
        : _store(NONE) {}

    Value(None)
        : _store(NONE) {}

    Value(Array v)
        : _store(v) {}

    Value(Object m)
        : _store(m) {}

    Value(String s)
        : _store(s) {}

    Value(Integer d)
        : _store(d) {}

#ifndef __ck_freestanding__
    Value(Number d)
        : _store(d) {}
#endif

    Value(bool b)
        : _store(b) {}

    Value &operator=(None) {
        _store = NONE;
        return *this;
    }

    Value &operator=(Array v) {
        _store = v;
        return *this;
    }

    Value &operator=(Object m) {
        _store = m;
        return *this;
    }

    Value &operator=(String s) {
        _store = s;
        return *this;
    }

    Value &operator=(Integer d) {
        _store = d;
        return *this;
    }

#ifndef __ck_freestanding__
    Value &operator=(Number d) {
        _store = d;
        return *this;
    }
#endif

    Value &operator=(bool b) {
        _store = b;
        return *this;
    }

    bool isNull() const {
        return _store.is<None>();
    }

    bool isArray() const {
        return _store.is<Vec<Value>>();
    }

    bool isObject() const {
        return _store.is<Object>();
    }

    bool isStr() const {
        return _store.is<String>();
    }

    bool isInt() const {
        return _store.is<Integer>();
    }

#ifndef __ck_freestanding__
    bool isFloat() const {
        return _store.is<Number>();
    }
#endif

    bool isBool() const {
        return _store.is<bool>();
    }

    Array &asArray() {
        return _store.unwrap<Array>();
    }

    Array const &asArray() const {
        return _store.unwrap<Array>();
    }

    Object &asObject() {
        return _store.unwrap<Object>();
    }

    Object const &asObject() const {
        return _store.unwrap<Object>();
    }

    String asStr() const {
        return _store.visit(
            Visitor{
                [](None) {
                    return "null";
                },
                [](Vec<Value>) {
                    return "<array>";
                },
                [](Map<String, Value>) {
                    return "<object>";
                },
                [](String s) {
                    return s;
                },
                [](Integer i) {
                    return Io::format("{}", i).unwrap();
                },
#ifndef __ck_freestanding__
                [](Number d) {
                    return Io::format("{}", d).unwrap();
                },
#endif
                [](bool b) {
                    return b ? "true" : "false";
                },
            }
        );
    }

    isize asInt() const {
        return _store.visit(
            Visitor{
                [](Integer i) {
                    return i;
                },

#ifndef __ck_freestanding__
                [](Number d) {
                    return (isize)d;
                },
#endif

                [](bool b) {
                    return b ? 1 : 0;
                },
                [](auto) {
                    return 0;
                },
            }
        );
    }

#ifndef __ck_freestanding__
    f64 asFloat() const {
        return _store.visit(
            Visitor{
                [](Integer i) {
                    return (f64)i;
                },
                [](Number d) {
                    return d;
                },
                [](bool b) {
                    return b ? (Number)1.0 : (Number)0.0;
                },
                [](auto) {
                    return (Number)0;
                },
            }
        );
    }
#endif

    bool asBool() const {
        return _store.visit(
            Visitor{
                [](None) {
                    return false;
                },
                [](Vec<Value> v) {
                    return v.len() > 0;
                },
                [](Map<String, Value> m) {
                    return m.len() > 0;
                },
                [](String s) {
                    return s.len() > 0;
                },
                [](Integer i) {
                    return i != 0;
                },
#ifndef __ck_freestanding__
                [](Number d) {
                    return d != (Number)0;
                },
#endif
                [](bool b) {
                    return b;
                },
            }
        );
    }

    Value get(Str key) const {
        if (not isObject()) {
            return NONE;
        }
        return try$(asObject().get(key));
    }

    Value get(usize index) const {
        if (not isArray() or asArray().len() <= index) {
            return NONE;
        }
        return asArray()[index];
    }

    usize len() const {
        return _store.visit(
            Visitor{
                [](Vec<Value> v) {
                    return v.len();
                },
                [](Map<String, Value> m) {
                    return m.len();
                },
                [](String s) {
                    return s.len();
                },
                [](auto) {
                    return 0;
                },
            }
        );

        return 0;
    }

    void clear() {
        _store.visit(
            Visitor{
                [](auto &v) {
                    v = {};
                }
            }
        );
    }

    auto visit(auto visitor) const {
        return _store.visit(visitor);
    }

    template <typename T>
    Opt<T> take() {
        return _store.take<T>();
    }

    template <typename T>
    Opt<T> take() const {
        return _store.take<T>();
    }
};

Res<Value> parse(Io::SScan &s);

Res<Value> parse(Str s);

Res<> stringify(Io::Emit &emit, Value const &v);

Res<String> stringify(Value const &v);

} // namespace Web::Json

inline auto operator""_json(char const *str, usize len) {
    return Web::Json::parse({str, len}).unwrap();
}

template <>
struct Karm::Io::Formatter<Web::Json::Value> {
    Res<usize> format(Io::TextWriter &writer, Web::Json::Value value) {
        Io::Emit emit{writer};
        try$(Web::Json::stringify(emit, value));
        return Ok(emit.total());
    }
};
