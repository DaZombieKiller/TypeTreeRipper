#pragma once
#include "common.hpp"
#include "MemLabelId.hpp"
#include "dynamic_array.hpp"
#include <atomic>

template<Revision R, Variant V>
class TypeTree;

template<Revision R, Variant V>
struct TypeTreeNode;

template<Revision R, Variant V>
class TypeTreeIterator;

template<Revision R, Variant V>
struct TypeTreeShareableData;

DECLARE_REVISION(TypeTree, Revision::V2019_1_0);
DECLARE_REVISION(TypeTree, Revision::V2019_3_0);
DECLARE_REVISION(TypeTreeNode, Revision::V2019_1_0);
DECLARE_REVISION(TypeTreeIterator, Revision::V2019_1_0);
DECLARE_REVISION(TypeTreeShareableData, Revision::V2019_2_0);
DECLARE_REVISION(TypeTreeShareableData, Revision::V2022_3_0);

DEFINE_ENUM(TransferInstructionFlags, int32_t,
    kSerializeGameRelease = 0x0100);

DEFINE_ENUM_REVISION(TransferInstructionFlags, uint64_t, Revision::V2021_1_0,
    kSerializeGameRelease = 0x0100);

//
// 5.0
//

template<Revision R, Variant V>
struct TypeTreeNode
{
    int16_t m_Version;
    uint8_t m_Level;
    uint8_t m_IsArray;
    uint32_t m_TypeStrOffset;
    uint32_t m_NameStrOffset;
    int32_t m_ByteSize;
    int32_t m_Index;
    uint32_t m_MetaFlag;
};

template<Revision R, Variant V>
class TypeTree
{
    dynamic_array<R, V>::template type<TypeTreeNode<R, V>> m_Nodes;
    dynamic_array<R, V>::template type<char> m_StringBuffer;
    dynamic_array<R, V>::template type<uint32_t> m_ByteOffsets;
public:
    TypeTree(TypeTreeShareableData<R, V> *sharedType, MemLabelId<R, V> const &label)
        : m_Nodes(label, 1), m_StringBuffer(label), m_ByteOffsets(label)
    {
        m_Nodes[0].m_Index = -1;
        m_Nodes[0].m_ByteSize = -1;
    }

    TypeTreeShareableData<R, V> *GetData() const
    {
        return nullptr;
    }

    dynamic_array<R, V>::template type<TypeTreeNode<R, V>> const &Nodes() const
    {
        return m_Nodes;
    }

    dynamic_array<R, V>::template type<char> const &StringsBuffer() const
    {
        return m_StringBuffer;
    }
};

template<Revision R, Variant V>
class TypeTreeIterator
{
    TypeTree<R, V> const *m_TypeTree;
    uint64_t m_NodeIndex;
public:
    TypeTreeIterator(TypeTree<R, V> const *tree, TypeTreeShareableData<R, V> const *data, uint64_t nodeIndex) :
        m_TypeTree(tree),
        m_NodeIndex(nodeIndex)
    {
    }
};

//
// 2019.1
//

template<Revision R, Variant V>
class TypeTreeShareableData
{
    dynamic_array<R, V>::template type<TypeTreeNode<R, V>> m_Nodes;
    dynamic_array<R, V>::template type<char> m_StringBuffer;
    dynamic_array<R, V>::template type<uint32_t> m_ByteOffsets;
    std::atomic<int> m_RefCount;
    MemLabelId<R, V> const &m_MemLabel;
public:
    explicit TypeTreeShareableData(MemLabelId<R, V> const &label) :
        m_Nodes(label, 1),
        m_StringBuffer(label),
        m_ByteOffsets(label),
        m_MemLabel(label)
    {
        m_Nodes[0].m_Version = 1;
        m_Nodes[0].m_ByteSize = -1;
    }

    void SetGenerationFlags(TransferInstructionFlags<R, V> options)
    {
    }

    dynamic_array<R, V>::template type<TypeTreeNode<R, V>> const &Nodes() const
    {
        return m_Nodes;
    }

    dynamic_array<R, V>::template type<char> const &StringsBuffer() const
    {
        return m_StringBuffer;
    }

    void Retain()
    {
        m_RefCount++;
    }
};

DEFINE_REVISION(class, TypeTree, Revision::V2019_1_0)
{
    TypeTreeShareableData<R, V> *m_Data;
    TypeTreeShareableData<R, V> m_PrivateData;
public:
    TypeTree(TypeTreeShareableData<R, V> *sharedType, MemLabelId<R, V> const &label) :
        m_Data(sharedType),
        m_PrivateData(label)
    {
        sharedType->Retain();
    }

    TypeTreeShareableData<R, V> *GetData() const
    {
        return m_Data;
    }

    dynamic_array<R, V>::template type<TypeTreeNode<R, V>> const &Nodes() const
    {
        return GetData()->Nodes();
    }

    dynamic_array<R, V>::template type<char> const &StringsBuffer() const
    {
        return GetData()->StringsBuffer();
    }
};

DEFINE_REVISION(struct, TypeTreeNode, Revision::V2019_1_0)
{
    int16_t m_Version = 0;
    uint8_t m_Level = 0;
    uint8_t m_TypeFlags = 0;
    uint32_t m_TypeStrOffset = 0;
    uint32_t m_NameStrOffset = 0;
    int32_t m_ByteSize = 0;
    int32_t m_Index = 0;
    uint32_t m_MetaFlag = 0;
    uint64_t m_RefTypeHash = 0;

    enum ETypeFlags
    {
        kFlagIsArray = 1 << 0,
        kFlagIsManagedReference = 1 << 1,
        kFlagIsManagedReferenceRegistry = 1 << 2,
        kFlagIsArrayOfRefs = 1 << 3,
    };
};

DEFINE_REVISION(class, TypeTreeIterator, Revision::V2019_1_0)
{
    TypeTree<R, V> const *m_LinkedTypeTree;
    TypeTreeShareableData<R, V> const *m_TypeTreeData;
    uint64_t m_NodeIndex;
public:
    TypeTreeIterator(TypeTree<R, V> const *tree, TypeTreeShareableData<R, V> const *data, uint64_t nodeIndex) :
        m_LinkedTypeTree(tree),
        m_TypeTreeData(data),
        m_NodeIndex(nodeIndex)
    {
    }
};

//
// 2019.2
//

DEFINE_REVISION(class, TypeTreeShareableData, Revision::V2019_2_0)
{
    dynamic_array<R, V>::template type<TypeTreeNode<R, V>> m_Nodes;
    dynamic_array<R, V>::template type<char> m_StringBuffer;
    dynamic_array<R, V>::template type<uint32_t> m_ByteOffsets;
    TransferInstructionFlags<R, V> m_FlagsAtGeneration = {};
    std::atomic<int> m_RefCount = 1;
    MemLabelId<R, V> const &m_MemLabel;
public:
    explicit TypeTreeShareableData(MemLabelId<R, V> const &label) :
        m_Nodes(label, 1),
        m_StringBuffer(label),
        m_ByteOffsets(label),
        m_MemLabel(label)
    {
        m_Nodes[0].m_Version = 1;
        m_Nodes[0].m_ByteSize = -1;
    }

    void SetGenerationFlags(TransferInstructionFlags<R, V> options)
    {
        m_FlagsAtGeneration = options;
    }

    dynamic_array<R, V>::template type<TypeTreeNode<R, V>> const &Nodes() const
    {
        return m_Nodes;
    }

    dynamic_array<R, V>::template type<char> const &StringsBuffer() const
    {
        return m_StringBuffer;
    }

    void Retain()
    {
        m_RefCount++;
    }
};

//
// 2019.3
//

DEFINE_REVISION(class, TypeTree, Revision::V2019_3_0)
{
    class Pool;
    TypeTreeShareableData<R, V> *m_Data;
    Pool *m_ReferencedTypes = nullptr;
    bool m_PoolOwned = false;
public:
    TypeTree(TypeTreeShareableData<R, V> *sharedType, MemLabelId<R, V> const &label) :
        m_Data(sharedType)
    {
        sharedType->Retain();
    }

    TypeTreeShareableData<R, V> *GetData() const
    {
        return m_Data;
    }

    dynamic_array<R, V>::template type<TypeTreeNode<R, V>> const &Nodes() const
    {
        return GetData()->Nodes();
    }

    dynamic_array<R, V>::template type<char> const &StringsBuffer() const
    {
        return GetData()->StringsBuffer();
    }
};

//
// 2022.3
//

DEFINE_REVISION(class, TypeTreeShareableData, Revision::V2022_3_0)
{
    dynamic_array<R, V>::template type<TypeTreeNode<R, V>> m_Nodes;
    dynamic_array<R, V>::template type<uint8_t> m_Levels;
    dynamic_array<R, V>::template type<int32_t> m_NextIndex;
    dynamic_array<R, V>::template type<char> m_StringBuffer;
    dynamic_array<R, V>::template type<uint32_t> m_ByteOffsets;
    TransferInstructionFlags<R, V> m_FlagsAtGeneration = {};
    std::atomic<int> m_RefCount = 1;
    MemLabelId<R, V> m_MemLabel;
public:
    explicit TypeTreeShareableData(MemLabelId<R, V> const &label) :
        m_Nodes(label, 1),
        m_Levels(label, 1),
        m_NextIndex(label, 1),
        m_StringBuffer(label),
        m_ByteOffsets(label),
        m_MemLabel(label)
    {
        m_Nodes[0].m_Version = 1;
        m_Nodes[0].m_ByteSize = -1;
        m_Levels[0] = 0;
        m_NextIndex[0] = -1;
    }

    void SetGenerationFlags(TransferInstructionFlags<R, V> options)
    {
        m_FlagsAtGeneration = options;
    }

    dynamic_array<R, V>::template type<TypeTreeNode<R, V>> const &Nodes() const
    {
        return m_Nodes;
    }

    dynamic_array<R, V>::template type<char> const &StringsBuffer() const
    {
        return m_StringBuffer;
    }

    void Retain()
    {
        m_RefCount++;
    }
};
