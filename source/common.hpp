#pragma once
#include <array>
#include <optional>
#include <sstream>
#include <utility>

#define FOR_EACH_VARIANT(X) \
    X(Editor) \
    X(Runtime) \
    X(RuntimeDev)

enum class Variant
{
#define DEFINE_VARIANT_ENUM_ENTRY(Name) Name,
    FOR_EACH_VARIANT(DEFINE_VARIANT_ENUM_ENTRY)
#undef DEFINE_VARIANT_ENUM_ENTRY

    Count
};

template<typename StringViewType>
    requires std::convertible_to<StringViewType, std::string_view> || std::convertible_to<StringViewType, std::wstring_view>
std::optional<Variant> VariantStringToVariant(const StringViewType name)
{
    const auto convertString = [name](const auto names) -> std::optional<Variant>
    {
        for (const auto &[variantName, variant] : names)
        {
            if (name == variantName)
            {
                return variant;
            }
        }

        return std::nullopt;
    };

    if constexpr (std::convertible_to<StringViewType, std::string_view>)
    {
        constexpr std::array kVariantNames = {
#define DEFINE_VARIANT_NAME_A_ENTRY(Name) std::make_tuple(#Name, Variant::Name),  
            FOR_EACH_VARIANT(DEFINE_VARIANT_NAME_A_ENTRY)
#undef DEFINE_VARIANT_NAME_A_ENTRY
        };

        return convertString(kVariantNames);
    }
    else
    {
        constexpr std::array kVariantNames = {
#define DEFINE_VARIANT_NAME_W_ENTRY(Name) std::make_tuple(L""#Name, Variant::Name),  
            FOR_EACH_VARIANT(DEFINE_VARIANT_NAME_W_ENTRY)
#undef DEFINE_VARIANT_NAME_W_ENTRY
        };

        return convertString(kVariantNames);
    }
}

inline std::optional<Variant> ExecutableNameToVariant(const std::string_view name)
{
    constexpr std::array kExecutableVariants = {
        std::make_tuple("Unity.exe", Variant::Editor),
    };

    for (const auto& [executableName, variant] : kExecutableVariants)
    {
        if (name == executableName)
        {
            return variant;
        }
    }

    return std::nullopt;
}

inline std::string_view VariantToString(const Variant variant)
{
    constexpr std::array kVariantNames = {
    #define DEFINE_VARIANT_STRING_ENTRY(Name) #Name,  
            FOR_EACH_VARIANT(DEFINE_VARIANT_STRING_ENTRY)
#undef DEFINE_VARIANT_STRING_ENTRY
    };

    return kVariantNames[std::to_underlying(variant)];
}

#undef FOR_EACH_VARIANT

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

using RevisionVersion = std::tuple<uint16_t, uint8_t, uint8_t>;

template <typename StringType, typename StringStreamType, auto Delimiter>
RevisionVersion ParseVersionString(const StringType &value)
{
    // spanstream doesn't exist in the android ndk :(
    StringStreamType versionParser(value);

    StringType versionComponent;
    std::getline(versionParser, versionComponent, Delimiter);
    const auto major = static_cast<uint16_t>(std::stoi(versionComponent));

    std::getline(versionParser, versionComponent, Delimiter);
    const auto minor = static_cast<uint8_t>(std::stoi(versionComponent));

    std::getline(versionParser, versionComponent, Delimiter);
    const auto patch = static_cast<uint8_t>(std::stoi(versionComponent));

    return std::make_tuple(major, minor, patch);
}

inline std::optional<Revision> VersionToRevision(const uint16_t major, const uint8_t minor, const uint8_t patch)
{
    constexpr std::array kRevisionVersions = {
#define DEFINE_REVISION_VERSION_ENTRY(major, minor, patch) std::make_tuple(major, minor, patch, Revision::V##major##_##minor##_##patch), 
        FOR_EACH_REVISION(DEFINE_REVISION_VERSION_ENTRY)
#undef DEFINE_REVISION_VERSION_ENTRY
    };

    std::optional<Revision> versionRevision;

    for (const auto &[revisionMajor, revisionMinor, revisionPatch, revision] : kRevisionVersions)
    {
        if (major > revisionMajor
            || (major == revisionMajor && (minor > revisionMinor
                || (minor == revisionMinor && patch >= revisionPatch))))
        {
            versionRevision = revision;
        }
    }

    return versionRevision;
}

template<typename StringType>
    requires std::convertible_to<StringType, std::string> || std::convertible_to<StringType, std::wstring>
std::optional<Revision> VersionStringToRevision(const StringType& value)
{
    RevisionVersion versionComponents;
    
    if constexpr (std::convertible_to<StringType, std::string>)
    {
        versionComponents = ParseVersionString<std::string, std::istringstream, '.'>(value);
    }
    else
    {
        versionComponents = ParseVersionString<std::wstring, std::basic_istringstream<wchar_t>, L'.'>(value);
    }

    const auto& [major, minor, patch] = versionComponents;
    return VersionToRevision(major, minor, patch);
}

inline RevisionVersion RevisionToVersion(const Revision revision)
{
    constexpr std::array kRevisionVersions = {
#define DEFINE_REVISION_VERSION_TUPLE_ENTRY(major, minor, patch) std::make_tuple<uint16_t, uint8_t, uint8_t>(major, minor, patch), 
        FOR_EACH_REVISION(DEFINE_REVISION_VERSION_TUPLE_ENTRY)
#undef DEFINE_REVISION_VERSION_ENTRY
    };

    return kRevisionVersions[std::to_underlying(revision)];
}

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
