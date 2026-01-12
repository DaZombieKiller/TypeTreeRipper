#pragma once
#include <cstdint>
#include <fstream>
#include <vector>

#include "common.hpp"
#include "RTTI.hpp"
#include "TypeTree.hpp"

//
// .ttbin binary layout:
// TypeTreeHeader
// TypeTreeRTTI[RTTICount]
// TypeTreeNode[NodesCount]
//

struct DumpedTypeTreeHeader
{
    // 'TYPETREE' in little-endian
    static constexpr auto kDefaultMagic = 0x4545525445505954;

    uint64_t Magic;
    uint32_t Version;

    // Revision
    uint16_t MajorRevision;
    uint8_t MinorRevision;
    uint8_t PatchRevision;

    std::string Variant;
};

struct DumpedTypeTreeRTTI
{
    std::string ClassName;
    std::string ClassNamespace;
    std::string Module;
    int32_t PersistentTypeID;
    int32_t Size;

    enum Flags : uint32_t
    {
        kRTTIFlagsIsAbstract = 1 << 0,
        kRTTIFlagsIsSealed = 1 << 1,
        kRTTIFlagsIsEditorOnly = 1 << 2,
        kRTTIFlagsIsStripped = 1 << 3,
        kRTTIFlagsIsDeprecated  = 1 << 4,
    };
    std::underlying_type_t<Flags> Flags;

    uint32_t BasePersistentTypeID;
    uint32_t DerivedFromTypeIndex;
    uint32_t DerivedFromDescendantCount;
};

struct DumpedTypeTreeNode
{
    std::string Type;
    std::string Name;

    enum Flags : uint32_t
    {
        kNodeFlagsIsArray = 1 << 0,
        kNodeFlagsIsManagedReference = 1 << 1,
        kNodeFlagsIsManagedReferenceRegistry = 1 << 2,
        kNodeFlagsIsArrayOfRefs = 1 << 3,
    };
    std::underlying_type_t<Flags> Flags;
    int32_t ByteSize;
    int32_t Index;

    int16_t Version;
    uint8_t Level;
    uint32_t MetaFlags;
    uint64_t RefTypeHash;
};

struct DumpedTypeTree
{
    DumpedTypeTreeRTTI RTTI;

    enum TransferInstructionFlags : uint64_t
    {
        kTransferFlagSerializeGameRelease = 0x0100,
    };
    std::underlying_type_t<TransferInstructionFlags> TransferFlags;

    std::vector<DumpedTypeTreeNode> Nodes;
};

namespace internal
{
    template<typename T>
    inline void Write(std::ofstream &output, const T &value)
    {
        // needed to workaround clang bug(?)
        // ReSharper disable once CppStaticAssertFailure
        static_assert(false, "No default specialization available for Write()");
    }

    template<typename T>
    inline void Write(std::ofstream &output, const std::vector<T> &values)
    {
        Write(output, static_cast<uint32_t>(values.size()));
        for (const auto &value : values)
        {
            Write(output, value);
        }
    }

    template<>
    inline void Write(std::ofstream &output, const int32_t &value)
    {
        output.write(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    template<>
    inline void Write(std::ofstream &output, const int16_t &value)
    {
        output.write(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    template<>
    inline void Write(std::ofstream &output, const uint64_t &value)
    {
        output.write(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    template<>
    inline void Write(std::ofstream &output, const uint32_t &value)
    {
        output.write(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    template<>
    inline void Write(std::ofstream &output, const uint16_t &value)
    {
        output.write(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    template<>
    inline void Write(std::ofstream &output, const uint8_t &value)
    {
        output.write(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    template<>
    inline void Write(std::ofstream &output, const std::string &value)
    {
        const auto size = static_cast<uint32_t>(value.size());
        Write(output, size);
        output.write(value.c_str(), size);
    }

    template<>
    inline void Write(std::ofstream &output, const DumpedTypeTreeHeader &value)
    {
        Write(output, value.Magic);
        Write(output, value.Version);
        Write(output, value.MajorRevision);
        Write(output, value.MinorRevision);
        Write(output, value.PatchRevision);
        Write(output, value.Variant);
    }

    template<>
    inline void Write(std::ofstream &output, const DumpedTypeTreeRTTI &value)
    {
        Write(output, value.ClassName);
        Write(output, value.ClassNamespace);
        Write(output, value.Module);
        Write(output, value.PersistentTypeID);
        Write(output, value.Size);
        Write(output, value.Flags);
        Write(output, value.BasePersistentTypeID);
        Write(output, value.DerivedFromTypeIndex);
        Write(output, value.DerivedFromDescendantCount);
    }

    template<>
    inline void Write(std::ofstream &output, const DumpedTypeTreeNode &value)
    {
        Write(output, value.Type);
        Write(output, value.Name);
        Write(output, value.Flags);
        Write(output, value.ByteSize);
        Write(output, value.Index);
        Write(output, value.Version);
        Write(output, value.Level);
        Write(output, value.MetaFlags);
        Write(output, value.RefTypeHash);
    }

    template<>
    inline void Write(std::ofstream &output, const DumpedTypeTree &value)
    {
        Write(output, value.RTTI);
        Write(output, value.TransferFlags);
        Write(output, value.Nodes);
    }
}

template<Revision R, Variant V>
class DumpedTypeTreeWriter
{
    using RTTI = ::RTTI<R, V>;
    using TypeTree = ::TypeTree<R, V>;
    using TypeTreeShareableData = ::TypeTreeShareableData<R, V>;
    using TypeTreeNode = ::TypeTreeNode<R, V>;
    using TransferInstructionFlags = ::TransferInstructionFlags<R, V>;

#define IF_HAS_MEMBER(Var, MemberName) if constexpr (requires { (Var).MemberName; })
#define IF_HAS_MEMBER_PTR(Var, MemberName) if constexpr (requires { (Var)->MemberName; })

    static void ConvertRTTI(const RTTI* rtti, DumpedTypeTreeRTTI &dumpedRtti)
    {
        dumpedRtti = {};

        dumpedRtti.ClassName = rtti->className;
        dumpedRtti.PersistentTypeID = rtti->persistentTypeID;
        dumpedRtti.Size = rtti->size;

        if (rtti->base)
            dumpedRtti.BasePersistentTypeID = rtti->base->persistentTypeID;

        if (rtti->isAbstract)
            dumpedRtti.Flags |= DumpedTypeTreeRTTI::kRTTIFlagsIsAbstract;

        if (rtti->isSealed)
            dumpedRtti.Flags |= DumpedTypeTreeRTTI::kRTTIFlagsIsAbstract;

        IF_HAS_MEMBER_PTR(rtti, isDeprecated)
        {
            if (rtti->isDeprecated)
                dumpedRtti.Flags |= DumpedTypeTreeRTTI::kRTTIFlagsIsDeprecated;
        }

        IF_HAS_MEMBER_PTR(rtti, isEditorOnly)
        {
            if (rtti->isEditorOnly)
                dumpedRtti.Flags |= DumpedTypeTreeRTTI::kRTTIFlagsIsEditorOnly;
        }

        IF_HAS_MEMBER_PTR(rtti, isStripped)
        {
            if (rtti->isStripped)
                dumpedRtti.Flags |= DumpedTypeTreeRTTI::kRTTIFlagsIsStripped;
        }

        IF_HAS_MEMBER_PTR(rtti, derivedFromInfo)
        {
            dumpedRtti.DerivedFromTypeIndex = rtti->derivedFromInfo.typeIndex;
            dumpedRtti.DerivedFromDescendantCount = rtti->derivedFromInfo.descendantCount;
        }

        IF_HAS_MEMBER_PTR(rtti, classNamespace)
        {
            dumpedRtti.ClassNamespace = rtti->classNamespace;
        }

        IF_HAS_MEMBER_PTR(rtti, module)
        {
            dumpedRtti.Module = rtti->module;
        }

        // NOTE: Do we also want to output attributes?
    }

    static void ConvertNode(const TypeTreeNode& node, DumpedTypeTreeNode& dumpedNode, char const* stringBuffer, char const* commonStringBuffer)
    {
        const auto getStringBufferString = [stringBuffer, commonStringBuffer](const uint32_t offset) -> std::string
        {
            if ((offset & 0x80000000) == 0)
            {
                return { stringBuffer + offset };
            }
            
            return { commonStringBuffer + (offset & ~0x80000000) };
        };

        dumpedNode = {};

        dumpedNode.Name = getStringBufferString(node.m_NameStrOffset);
        dumpedNode.Type = getStringBufferString(node.m_TypeStrOffset);
        dumpedNode.ByteSize = node.m_ByteSize;
        dumpedNode.Index = node.m_Index;
        dumpedNode.Level = node.m_Level;
        dumpedNode.Version = node.m_Version;

        // TODO: Add proper parsing for these
        dumpedNode.MetaFlags = node.m_MetaFlag;

        IF_HAS_MEMBER(node, m_IsArray)
        {
            dumpedNode.Flags |= DumpedTypeTreeNode::kNodeFlagsIsArray;
        }

        IF_HAS_MEMBER(node, m_TypeFlags)
        {
            if (node.m_TypeFlags & TypeTreeNode::kFlagIsArray)
                dumpedNode.Flags |= DumpedTypeTreeNode::kNodeFlagsIsArray;

            if (node.m_TypeFlags & TypeTreeNode::kFlagIsManagedReference)
                dumpedNode.Flags |= DumpedTypeTreeNode::kNodeFlagsIsManagedReference;

            if (node.m_TypeFlags & TypeTreeNode::kFlagIsManagedReferenceRegistry)
                dumpedNode.Flags |= DumpedTypeTreeNode::kNodeFlagsIsManagedReferenceRegistry;

            if (node.m_TypeFlags & TypeTreeNode::kFlagIsArrayOfRefs)
                dumpedNode.Flags |= DumpedTypeTreeNode::kNodeFlagsIsArrayOfRefs;
        }

        IF_HAS_MEMBER(node, m_RefTypeHash)
        {
            dumpedNode.RefTypeHash = node.m_RefTypeHash;
        }
    }
#undef IF_HAS_MEMBER

public:
    void Add(const RTTI* rtti, const TypeTree& tree, const TransferInstructionFlags& flags, char const* commonStringBuffer)
    {
        DumpedTypeTree dumpedTree{};
        ConvertRTTI(rtti, dumpedTree.RTTI);

        // TODO: Add proper parsing for these
        if (flags & TransferInstructionFlags::kSerializeGameRelease)
        {
            dumpedTree.TransferFlags |= DumpedTypeTree::kTransferFlagSerializeGameRelease;
        }

        if (!rtti->isAbstract && rtti->factory)
        {
            const auto stringBuffer = tree.StringsBuffer();
            const auto nodes = tree.Nodes();

            dumpedTree.Nodes = std::vector<DumpedTypeTreeNode>(nodes.size());
            for (size_t i = 0; i < nodes.size(); i++)
            {
                ConvertNode(nodes[i], dumpedTree.Nodes[i], stringBuffer.data(), commonStringBuffer);
            }
        }

        TypeTrees.push_back(std::move(dumpedTree));
    }

    void Write(std::ofstream &output) const
    {
        const auto &[major, minor, patch] = RevisionToVersion(R);

        internal::Write(output, DumpedTypeTreeHeader{
            .Magic = DumpedTypeTreeHeader::kDefaultMagic,
            .Version = 1,
            .MajorRevision = major,
            .MinorRevision = minor,
            .PatchRevision = patch,
            .Variant = std::string(VariantToString(V)),
        });

        internal::Write(output, TypeTrees);
    }
private:
    std::vector<DumpedTypeTree> TypeTrees;
};
