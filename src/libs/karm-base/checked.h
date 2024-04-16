#pragma once

#include <karm-meta/traits.h>

#include "limits.h"
#include "res.h"

namespace Karm {

template <typename T>
bool willAddOverflow(T lhs, T rhs) {
    return (lhs > 0 and rhs > 0) and (lhs > Limits<T>::MAX - rhs or Limits<T>::MAX - lhs < rhs);
}

template <typename T>
bool willAddUnderflow(T lhs, T rhs) {
    return (lhs < 0 and rhs < 0) and (lhs > Limits<T>::MIN - rhs or Limits<T>::MIN - lhs < rhs);
}

template <typename T>
bool willSubOverflow(T lhs, T rhs) {
    return (lhs > 0 and rhs < 0) and (lhs > Limits<T>::MAX - rhs);
}

template <typename T>
bool willSubUnderflow(T lhs, T rhs) {
    return (lhs < 0 and rhs > 0) and (lhs < Limits<T>::MIN - rhs);
}

template <typename T>
Res<T> checkedAdd(T lhs, T rhs) {
    if (willAddOverflow(lhs, rhs))
        return Error::other("numeric overflow");
    if (willAddUnderflow(lhs, rhs))
        return Error::other("numeric underflow");
    return Ok(lhs + rhs);
}

template <typename T>
T saturatingAdd(T lhs, T rhs) {
    if (willAddOverflow(lhs, rhs))
        return Limits<T>::MAX;
    if (willAddUnderflow(lhs, rhs))
        return Limits<T>::MIN;
    return lhs + rhs;
}

template <typename T>
Res<T> checkedSub(T lhs, T rhs) {
    if (willSubOverflow(lhs, rhs))
        return Error::other("numeric overflow");
    if (willSubUnderflow(lhs, rhs))
        return Error::other("numeric underflow");
    return Ok(lhs - rhs);
}

template <typename T>
T saturatingSub(T lhs, T rhs) {
    if (willSubOverflow(lhs, rhs))
        return Limits<T>::MAX;
    if (willSubUnderflow(lhs, rhs))
        return Limits<T>::MIN;
    return lhs - rhs;
}

template <typename T>
Res<T> checkedInc(T val) {
    if (willAddOverflow(val, 1))
        return Error::other("numeric overflow");
    return Ok(val + 1);
}

template <typename T>
T saturatingInc(T val) {
    if (willAddOverflow(val, 1))
        return Limits<T>::MAX;
    return val + 1;
}

template <typename T>
Res<T> checkedDec(T op) {
    if (op == Limits<T>::MIN)
        return Error::other("numeric underflow");
    return Ok(op - 1);
}

template <typename T>
T saturatingDec(T val) {
    if (val == Limits<T>::MIN)
        return Limits<T>::MIN;
    return val - 1;
}

} // namespace Karm
