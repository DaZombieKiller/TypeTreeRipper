#pragma once
#include <array>
#include <utility>

enum class Variant
{
    Editor,
    Runtime,
    RuntimeDev,
    Count
};

#define FOR_EACH_REVISION(X) \
    X(0, 0, 0) \
    X(5, 0, 0) \
    X(5, 1, 0) \
    X(5, 2, 0) \
    X(5, 4, 0) \
    X(5, 5, 0) \
    X(2017, 1, 0) \
    X(2017, 3, 0) \
    X(2017, 4, 0) \
    X(2017, 4, 18) \
    X(2017, 4, 30) \
    X(2018, 2, 0) \
    X(2018, 3, 0) \
    X(2019, 1, 0) \
    X(2019, 2, 0) \
    X(2019, 3, 0) \
    X(2019, 4, 0) \
    X(2019, 4, 34) \
    X(2019, 4, 40) \
    X(2021, 1, 0) \
    X(2021, 3, 0) \
    X(2022, 2, 0) \
    X(2022, 3, 0) \
    X(2023, 1, 0)

enum class Revision
{
#define DEFINE_REVISION_ENUM_ENTRY(major, minor, patch) V##major##_##minor##_##patch,
    FOR_EACH_REVISION(DEFINE_REVISION_ENUM_ENTRY)
#undef DEFINE_REVISION_ENUM_ENTRY

    Count,
};

constexpr std::array kRevisionVersions = {
#define DEFINE_REVISION_VERSION_ENTRY(major, minor, patch) std::make_tuple(major, minor, patch, Revision::V##major##_##minor##_##patch),
    FOR_EACH_REVISION(DEFINE_REVISION_VERSION_ENTRY)
#undef DEFINE_REVISION_VERSION_ENTRY
};

#undef FOR_EACH_REVISION

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
