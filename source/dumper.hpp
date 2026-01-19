#pragma once
#include "common.hpp"

#include <array>
#include <span>
#include <functional>
#include <algorithm>
#include <ranges>

#include "MemLabelId.hpp"
#include "Object.hpp"
#include "GenerateTypeTreeTransfer.hpp"
#include "platform_impl.hpp"
#include "dumper.hpp"
#include "executable.hpp"
#include "binary_output.hpp"

struct IDumper
{
    virtual void Run() = 0;
};

template<Revision R, Variant V, typename TPlatformImpl>
    requires IsPlatformImpl<R, V, TPlatformImpl>
class Dumper : public IDumper
{
    using Object = ::Object<R, V>;
    using TypeTree = ::TypeTree<R, V>;
    using TypeTreeShareableData = ::TypeTreeShareableData<R, V>;
    using TransferInstructionFlags = ::TransferInstructionFlags<R, V>;
    using MemLabelId = ::MemLabelId<R, V>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, V>;
    using RuntimeTypeArray = ::RuntimeTypeArray<R, V>;
    using RTTI = ::RTTI<R, V>;

    using DumpedTypeTreeWriter = ::DumpedTypeTreeWriter<R, V>;

    bool IsValidPointer(void const *ptr, const size_t size, const uint8_t expectedProtection = ExecutableSection::kSectionProtectionRead)
    {
        return std::ranges::any_of(PlatformImpl.GetExecutableSections(), [&](const ExecutableSection &section)
        {
            return section.IsValidPointer(ptr, size, expectedProtection);
        });
    }

    bool StringEquals(char const *p, const std::string_view other)
    {
        if (!IsValidPointer(p, other.size() + 1))
            return false;

        if (p[other.size()] != '\0')
            return false;

        return std::string_view(p, other.size()) == other;
    }

    bool IsValidRuntimeTypeArray(RuntimeTypeArray const *pArray)
    {
        if (!IsValidPointer(pArray, sizeof(RuntimeTypeArray)))
            return false;

        if constexpr (R >= Revision::V5_2_0)
        {
            if (pArray->Count < 2 || pArray->Count > pArray->Types.size())
                return false;

            for (int i = 0; i < 2; i++)
            {
                if (!IsValidPointer(pArray->Types[i], sizeof(RTTI)))
                {
                    return false;
                }

                if (pArray->Types[i]->factory && !IsValidPointer(reinterpret_cast<void const *>(pArray->Types[i]->factory), 1))
                {
                    return false;
                }
            }

            if (pArray->Types[0]->persistentTypeID != 0)
                return false;

            if (pArray->Types[1]->base != pArray->Types[0])
                return false;

            if (!StringEquals(pArray->Types[0]->className, "Object"))
                return false;

            return true;
        }

        return false;
    }

    RuntimeTypeArray const *GetRuntimeTypeArray()
    {
        PlatformImpl.DebugLog("Retrieving RuntimeTypeArray");

        for (const auto &section : PlatformImpl.GetExecutableSections() | std::views::filter([](const ExecutableSection &x)
        {
            // The runtime type array is initialized at runtime, if the section
               // is not writable then it can not contain the runtime type array.
            if ((x.Protection & ExecutableSection::kSectionProtectionWrite) == 0)
                return false;

            if ((x.Protection & ExecutableSection::kSectionProtectionRead) == 0)
                return false;

            return true;
        }))
        {
            for (size_t i = 0; i < section.Data.size() - sizeof(RuntimeTypeArray); i += sizeof(uintptr_t))
            {
                auto pArray = reinterpret_cast<RuntimeTypeArray const *>(section.Data.data() + i);

                if (!IsValidRuntimeTypeArray(pArray))
                    continue;

                return pArray;
            }
        }

        PlatformImpl.DebugLog("Failed to find RuntimeTypeArray");

        return nullptr;
    }

    char const *GetCommonStringBuffer()
    {
        PlatformImpl.DebugLog("Retrieving common string buffer");

        static constexpr auto kCommonStringBufferPattern = std::span("AABB\0AnimationClip");
        const auto searcher = std::boyer_moore_horspool_searcher(std::cbegin(kCommonStringBufferPattern), std::cend(kCommonStringBufferPattern));

        for (const auto &section : PlatformImpl.GetExecutableSections())
        {
            if ((section.Protection & ExecutableSection::kSectionProtectionRead) == 0)
                continue;

            const auto region = section.Data;
            if (const auto result = std::search(std::cbegin(region), std::cend(region), searcher);
                result != std::cend(region))
            {
                return region.data() + std::distance(std::cbegin(region), result);
            }
        }

        return nullptr;
    }
public:
    void Run() override
    {
        PlatformImpl.DebugLog("Dumper started");

        if constexpr (R >= Revision::V5_2_0)
        {
            auto pArray = GetRuntimeTypeArray();
            auto pTable = GetCommonStringBuffer();

            const auto dumpTypes = [&](const TransferInstructionFlags &flags, const std::string_view outputName)
            {
                for (int i = 0; i < pArray->Count; i++)
                {
                    PlatformImpl.DebugLog((std::string("Processing type ") + pArray->Types[i]->className).c_str());

                    RTTI *pRTTI = pArray->Types[i];

                    MemLabelId label;
                    TypeTreeShareableData data(label);
                    TypeTree tree(&data, label);

                    if (!pRTTI->isAbstract && pRTTI->factory)
                    {
                        Object *object = pRTTI->factory(label, kCreateObjectDefault);
                        GenerateTypeTreeTransfer transfer(tree, flags, object, pRTTI->size);
                        object->VirtualRedirectTransfer(transfer);
                    }

                    Writer.Add(pRTTI, tree, flags, pTable);
                }

                PlatformImpl.DebugLog("Dumped types, now writing to file");

                auto outputStream = PlatformImpl.CreateOutputFile(outputName.data());
                Writer.Write(outputStream);
            };

            // Always dump release types
            dumpTypes(TransferInstructionFlags::kSerializeGameRelease, "release.ttbin");

            // If we are in an editor, also dump the editor types
            if constexpr (V == Variant::Editor)
            {
                dumpTypes(TransferInstructionFlags::kNone, "editor.ttbin");
            }
        }
    }

    static Dumper Instance;
private:
    TPlatformImpl PlatformImpl{};
    DumpedTypeTreeWriter Writer{};
};

template<Revision R, Variant V, typename TPlatformImpl>
    requires IsPlatformImpl<R, V, TPlatformImpl>
Dumper<R, V, TPlatformImpl> Dumper<R, V, TPlatformImpl>::Instance;

template<Variant V, template<Revision, Variant> typename TPlatformImpl>
class DumperArray
{
    // Layer of indirection added so that we can static_assert on the platform implementation concept
    template<Revision R>
    struct DumperInstantiationHelper
    {
        static_assert(IsPlatformImpl<R, V, TPlatformImpl<R, V>>, "Dumper platform implementation is ill-formed");
        using DumperType = Dumper<R, V, TPlatformImpl<R, V>>;
    };

public:
    static constexpr auto Instances = []<size_t... I>(std::index_sequence<I...>)
    {
        return std::array<IDumper *, sizeof...(I)>{ &DumperInstantiationHelper<static_cast<Revision>(I)>::DumperType::Instance... };
    }(std::make_index_sequence<std::to_underlying(Revision::Count)>{});
};

template<template<Revision, Variant> typename TPlatformImpl>
static constexpr auto DumperVariantInstances = []<size_t... I>(std::index_sequence<I...>)
{
    return std::array<const std::array<IDumper *, std::to_underlying(Revision::Count)>, sizeof...(I)>{ DumperArray<static_cast<Variant>(I), TPlatformImpl>::Instances... };
}(std::make_index_sequence<std::to_underlying(Variant::Count)>{});

template<template<Revision, Variant> typename TPlatformImpl>
void RunDumper(const Revision revision, const Variant variant)
{
    const auto intRevision = std::to_underlying(revision);
    const auto intVariant = std::to_underlying(variant);

    return DumperVariantInstances<TPlatformImpl>[intVariant][intRevision]->Run();
}

