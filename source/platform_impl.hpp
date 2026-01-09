#pragma once
#include "common.hpp"
#include "RTTI.hpp"

template<Revision R, Variant V, typename T>
concept IsPlatformImpl = requires(T impl)
{   
    { impl.GetRuntimeTypeArray() } -> std::convertible_to<::RuntimeTypeArray<R, V> const *>;
    { impl.GetCommonStringBuffer() } -> std::convertible_to<char const *>;
};
