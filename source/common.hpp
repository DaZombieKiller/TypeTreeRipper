#pragma once
#include <utility>

enum class Variant
{
    Editor,
    Runtime,
    RuntimeDev,
};

enum class Revision
{
    V0,
    V3_0,
    V3_4,
    V3_5,
    V4_0,
    V5_0,
    V5_1,
    V5_2,
    V5_3,
    V5_4,
    V5_5,
    V2017_1,
    V2017_3,
    V2018_2,
    V2018_3,
    V2019_1,
    V2019_2,
    V2019_3,
    V2019_4,
    V2020_1,
    V2021_1,
    V2022_1,
    V2022_2,
    V2022_3,
    V2023_1,
    V6000_0,
    Count,
};

namespace details
{
    template<template<Revision, Variant> typename T, Revision R, Variant V>
    struct HasExplicitRevision : std::false_type {};

    template<template<Revision, Variant> typename T, Revision R, Variant V>
    consteval Revision GetNextExplicitRevisionRecursive()
    {
        if constexpr (R >= Revision::Count)
        {
            return Revision::Count;
        }
        else if constexpr (HasExplicitRevision<T, R, V>::value)
        {
            return R;
        }
        else
        {
            return GetNextExplicitRevisionRecursive<T, static_cast<Revision>(static_cast<int>(R) + 1), V>();
        }
    }

    template<template<Revision, Variant> typename T, Revision R, Variant V>
    consteval Revision GetNextExplicitRevision()
    {
        return GetNextExplicitRevisionRecursive<T, static_cast<Revision>(static_cast<int>(R) + 1), V>();
    }

    template<template<Revision, Variant> typename T, Revision R, Variant V, Revision Min>
    concept ExplicitRevision = R >= Min && R < GetNextExplicitRevision<T, Min, V>();
}

#define DECLARE_REVISION(Type, Rev) \
    template<Variant V> struct ::details::HasExplicitRevision<Type, Rev, V> : ::std::true_type {}

#define DECLARE_REVISION_VARIANT(Type, Rev, Var) \
    template<> struct ::details::HasExplicitRevision<Type, Rev, Var> : ::std::true_type {}

#define DEFINE_REVISION(T, Type, Rev) \
    template<Revision R, Variant V> \
    requires ::details::ExplicitRevision<Type, R, V, Rev> \
    T Type<R, V>

#define DEFINE_REVISION_VARIANT(T, Type, Rev, Var) \
    template<Revision R> \
    requires ::details::ExplicitRevision<Type, R, Var, Rev> \
    T Type<R, Var>

#define DEFINE_ENUM(name, base, ...) \
    namespace enum_details \
    { \
        template<Revision R, Variant V> \
        struct name \
        { \
            enum type : base \
            { \
                __VA_ARGS__ \
            }; \
        }; \
    } \
    \
    template<Revision R, Variant V> \
    using name = enum_details::name<R, V>::type

#define DEFINE_ENUM_REVISION(name, base, version, ...) \
    DECLARE_REVISION(enum_details::name, version); \
    namespace enum_details \
    { \
        DEFINE_REVISION(struct, name, version) \
        { \
            enum type : base \
            { \
                __VA_ARGS__ \
            }; \
        }; \
    }

#define DEFINE_ENUM_REVISION_VARIANT(name, base, version, variant, ...) \
    DECLARE_REVISION_VARIANT(enum_details::name, version, variant); \
    namespace enum_details \
    { \
        DEFINE_REVISION_VARIANT(struct, name, version, variant) \
        { \
            enum type : base \
            { \
                __VA_ARGS__ \
            }; \
        }; \
    }
