#pragma once
#include "common.hpp"

#include <array>
#include <fstream>
#include "MemLabelId.hpp"
#include "Object.hpp"
#include "GenerateTypeTreeTransfer.hpp"
#include "platform_impl.hpp"
#include "dumper.hpp"

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

    void DumpNodes(TypeTreeShareableData &data, char const *commonString, std::ofstream &ofs)
    {
        int minLevel = INT_MAX;

        for (int n = 0; n < data.Nodes().size(); n++)
        {
            auto &node = data.Nodes()[n];

            if (node.m_Level < minLevel)
            {
                minLevel = node.m_Level;
            }
        }

        for (int n = 0; n < data.Nodes().size(); n++)
        {
            auto &node = data.Nodes()[n];

            for (int ind = 0; ind < node.m_Level - minLevel; ind++)
                ofs << '\t';

            if ((node.m_TypeStrOffset & 0x80000000) == 0)
            {
                ofs << data.StringsBuffer().data() + node.m_TypeStrOffset;
            }
            else
            {
                ofs << commonString + (node.m_TypeStrOffset & ~0x80000000);
            }

            ofs << " ";

            if ((node.m_NameStrOffset & 0x80000000) == 0)
            {
                ofs << data.StringsBuffer().data() + node.m_NameStrOffset;
            }
            else
            {
                ofs << commonString + (node.m_NameStrOffset & ~0x80000000);
            }

            ofs.put(ofs.widen('\n'));
        }
    }
public:
    void Run() override
    {
        if constexpr (R >= Revision::V5_2)
        {
            auto pArray = PlatformImpl.GetRuntimeTypeArray();
            auto pTable = PlatformImpl.GetCommonStringBuffer();
            std::ofstream ofs("types.txt");

            for (int i = 0; i < pArray->Count; i++)
            {
                RTTI *pRTTI = pArray->Types[i];

                if (pRTTI->isAbstract || !pRTTI->factory)
                    continue;

                MemLabelId label;
                Object *object = pRTTI->factory(label, kCreateObjectDefault);
                TypeTreeShareableData data(label);
                TypeTree tree(&data, label);
                GenerateTypeTreeTransfer transfer(tree, TransferInstructionFlags::kSerializeGameRelease, object, pRTTI->size);
                object->VirtualRedirectTransfer(transfer);
                DumpNodes(data, pTable, ofs);
            }
        }
    }

    static Dumper Instance;
private:
    TPlatformImpl PlatformImpl{};
};

template<Revision R, Variant V, typename TPlatformImpl>
    //requires std::is_base_of_v<IPlatformImpl<R, V>, TPlatformImpl>
    requires IsPlatformImpl<R, V, TPlatformImpl>
Dumper<R, V, TPlatformImpl> Dumper<R, V, TPlatformImpl>::Instance;

template<Variant V, template<Revision PlatformImplR, Variant PlatformImplV> typename TPlatformImpl>
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
    }(std::make_index_sequence<static_cast<size_t>(Revision::Count)>{});
};

template<template<Revision R, Variant V> typename TPlatformImpl>
void RunDumper(const Revision revision, const Variant variant)
{
    const auto intRevision = std::to_underlying(revision);

    switch (variant)
    {
    case Variant::Editor:
        DumperArray<Variant::Editor, TPlatformImpl>::Instances[intRevision]->Run();
        break;
    case Variant::Runtime:
        DumperArray<Variant::Runtime, TPlatformImpl>::Instances[intRevision]->Run();
        break;
    case Variant::RuntimeDev:
        DumperArray<Variant::RuntimeDev, TPlatformImpl>::Instances[intRevision]->Run();
        break;
    }
}

