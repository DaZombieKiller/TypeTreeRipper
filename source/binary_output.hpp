#pragma once
#include <cstdint>
#include <fstream>
#include <vector>

#include "common.hpp"
#include "RTTI.hpp"
#include "TypeTree.hpp"
#undef max

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
    static constexpr auto kInvalid = std::numeric_limits<uint32_t>::max();

    std::string ClassName;
    std::string ClassNamespace;
    std::string Module;
    int32_t PersistentTypeID;
    int32_t Size;

    enum Flags : uint32_t
    {
        kRTTIFlagIsAbstract = 1 << 0,
        kRTTIFlagIsSealed = 1 << 1,
        kRTTIFlagIsEditorOnly = 1 << 2,
        kRTTIFlagIsStripped = 1 << 3,
        kRTTIFlagIsDeprecated  = 1 << 4,
    };
    std::underlying_type_t<Flags> Flags;

    uint32_t BasePersistentTypeID = kInvalid;
    uint32_t DerivedFromTypeIndex = kInvalid;
    uint32_t DerivedFromDescendantCount = kInvalid;
};

struct DumpedTypeTreeNode
{
    std::string Type;
    std::string Name;

    enum Flags : uint32_t
    {
        kNodeFlagIsArray = 1 << 0,
        kNodeFlagIsManagedReference = 1 << 1,
        kNodeFlagIsManagedReferenceRegistry = 1 << 2,
        kNodeFlagIsArrayOfRefs = 1 << 3,
    };
    std::underlying_type_t<Flags> Flags;
    int32_t ByteSize;
    int32_t Index;

    int16_t Version;
    uint8_t Level;

    enum MetaFlags : uint32_t
    {
        kNodeMetaFlagHideInEditor = 1 << 0,
        // ?
        // ?
        // ?
        kNodeMetaFlagNotEditable = 1 << 4,
        kNodeMetaFlagReorderable = 1 << 5,
        kNodeMetaFlagStrongPPtr = 1 << 6,
        // ?
        kNodeMetaFlagTreatIntegerValueAsBoolean = 1 << 8,
        // ?
        // ?
        kNodeMetaFlagSimpleEditor = 1 << 11,
        kNodeMetaFlagDebugProperty = 1 << 12,
        // ?
        kNodeMetaFlagAlignBytes = 1 << 14,
        kNodeMetaFlagAnyChildUsesAlignBytes = 1 << 15,
        kNodeMetaFlagIgnoreWithInspectorUndo = 1 << 16,
        // ?
        kNodeMetaFlagEditorDisplaysCharacterMap = 1 << 18,
        kNodeMetaFlagIgnoreInMetaFiles = 1 << 19,
        kNodeMetaFlagTransferAsArrayEntryNameInMetaFiles = 1 << 20,
        kNodeMetaFlagTransferUsingFlowMappingStyle = 1 << 21,
        kNodeMetaFlagGenerateBitwiseDifferences = 1 << 22,
        kNodeMetaFlagDontAnimate = 1 << 23,
        kNodeMetaFlagTransferHex64 = 1 << 24,
        kNodeMetaFlagCharProperty = 1 << 25,
        kNodeMetaFlagDontValidateUTF8 = 1 << 26,
        kNodeMetaFlagFixedBuffer = 1 << 27,
        kNodeMetaFlagDisallowSerializedPropertyModification = 1 << 28
    };
    std::underlying_type_t<MetaFlags> MetaFlags;
    uint64_t RefTypeHash;
};

enum DumpedTransferInstructionFlags : uint64_t
{
    kTransferFlagReadWriteFromSerializedFile = 1 << 0,
    kTransferFlagAssetMetaDataOnly = 1 << 1,
    kTransferFlagHandleDrivenProperties = 1 << 2,
    kTransferFlagLoadAndUnloadAssetsDuringBuild = 1 << 3,
    kTransferFlagSerializeDebugProperties = 1 << 4,
    kTransferFlagIgnoreDebugPropertiesForIndex = 1 << 5,
    kTransferFlagBuildPlayerOnlySerializeBuildProperties = 1 << 6,
    kTransferFlagIsCloningObject = 1 << 7,
    kTransferFlagSerializeGameRelease = 1 << 8,
    kTransferFlagSwapEndianness = 1 << 9,
    kTransferFlagResolveStreamedResourceSources = 1 << 10,
    kTransferFlagDontReadObjectsFromDiskBeforeWriting = 1 << 11,
    kTransferFlagSerializeMonoReload = 1 << 12,
    kTransferFlagDontRequireAllMetaFlags = 1 << 13,
    kTransferFlagSerializeForPrefabSystem = 1 << 14,
    kTransferFlagSerializeForSlimPlayer = 1 << 15,
    kTransferFlagLoadPrefabAsScene = 1 << 16,
    kTransferFlagSerializeCopyPasteTransfer = 1 << 17,
    kTransferFlagSkipSerializeToTempFile = 1 << 18,
    kTransferFlagBuildResourceImage = 1 << 19,
    kTransferFlagDontWriteUnityVersion = 1 << 20,
    kTransferFlagSerializeEditorMinimalScene = 1 << 21,
    kTransferFlagGenerateBakedPhysixMeshes = 1 << 22,
    kTransferFlagThreadedSerialization = 1 << 23,
    kTransferFlagIsBuiltinResourcesFile = 1 << 24,
    kTransferFlagPerformUnloadDependencyTracking = 1 << 25,
    kTransferFlagDisableWriteTypeTree = 1 << 26,
    kTransferFlagAutoreplaceEditorWindow = 1 << 27,
    kTransferFlagDontCreateMonoBehaviorScriptWrapper = 1 << 28,
    kTransferFlagSerializeForInspector = 1 << 29,
    kTransferFlagSerializedAssetBundleVersion = 1 << 30,
    kTransferFlagAllowTextSerialization = 1 << 31,
    kTransferFlagIgnoreSerializeReferenceMissingType = 1LL << 32,
    kTransferFlagDontUpdateTransformRootOrderOnTypes = 1LL << 33,
    kTransferFlagSerializingForDevelopmentBuild = 1LL << 34,
    kTransferFlagSerializingFQN = 1LL << 35
};

struct DumpedTypeTree
{
    DumpedTypeTreeRTTI RTTI;
    std::underlying_type_t<DumpedTransferInstructionFlags> TransferFlags;

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
    using TransferMetaFlags = ::TransferMetaFlags<R, V>;

#define IF_HAS_MEMBER(Var, MemberName) if constexpr (requires { (Var).MemberName; })
#define IF_HAS_MEMBER_PTR(Var, MemberName) if constexpr (requires { (Var)->MemberName; })
#define IF_HAS_ENUM_FLAG(Var, EnumName, Result, ResultEnumName) if ((Var) & (EnumName)) (Result) |= ResultEnumName

    static void ConvertRTTI(const RTTI* rtti, DumpedTypeTreeRTTI &dumpedRtti)
    {
        dumpedRtti = {};

        dumpedRtti.ClassName = rtti->className;
        dumpedRtti.PersistentTypeID = rtti->persistentTypeID;
        dumpedRtti.Size = rtti->size;

        if (rtti->base)
            dumpedRtti.BasePersistentTypeID = rtti->base->persistentTypeID;

        if (rtti->isAbstract)
            dumpedRtti.Flags |= DumpedTypeTreeRTTI::kRTTIFlagIsAbstract;

        if (rtti->isSealed)
            dumpedRtti.Flags |= DumpedTypeTreeRTTI::kRTTIFlagIsSealed;

        IF_HAS_MEMBER_PTR(rtti, isDeprecated)
        {
            if (rtti->isDeprecated)
                dumpedRtti.Flags |= DumpedTypeTreeRTTI::kRTTIFlagIsDeprecated;
        }

        IF_HAS_MEMBER_PTR(rtti, isEditorOnly)
        {
            if (rtti->isEditorOnly)
                dumpedRtti.Flags |= DumpedTypeTreeRTTI::kRTTIFlagIsEditorOnly;
        }

        IF_HAS_MEMBER_PTR(rtti, isStripped)
        {
            if (rtti->isStripped)
                dumpedRtti.Flags |= DumpedTypeTreeRTTI::kRTTIFlagIsStripped;
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

        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kHideInEditorMask, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagHideInEditor);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kNotEditableMask, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagNotEditable);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kReorderable, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagReorderable);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kStrongPPtrMask, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagStrongPPtr);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kTreatIntegerValueAsBoolean, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagTreatIntegerValueAsBoolean);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kSimpleEditorMask, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagSimpleEditor);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kDebugPropertyMask, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagDebugProperty);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kAlignBytesFlag, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagAlignBytes);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kIgnoreWithInspectorUndoMask, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagIgnoreWithInspectorUndo);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kAnyChildUsesAlignBytesFlag, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagAnyChildUsesAlignBytes);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kEditorDisplaysCharacterMap, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagEditorDisplaysCharacterMap);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kIgnoreInMetaFiles, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagIgnoreInMetaFiles);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kTransferAsArrayEntryNameInMetaFiles, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagTransferAsArrayEntryNameInMetaFiles);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kTransferUsingFlowMappingStyle, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagTransferUsingFlowMappingStyle);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kGenerateBitwiseDifferences, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagGenerateBitwiseDifferences);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kDontAnimate, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagDontAnimate);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kTransferHex64, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagTransferHex64);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kCharPropertyMask, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagCharProperty);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kDontValidateUTF8, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagDontValidateUTF8);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kFixedBufferFlag, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagFixedBuffer);
        IF_HAS_ENUM_FLAG(node.m_MetaFlag, TransferMetaFlags::kDisallowSerializedPropertyModification, dumpedNode.MetaFlags, DumpedTypeTreeNode::kNodeMetaFlagDisallowSerializedPropertyModification);

        IF_HAS_MEMBER(node, m_IsArray)
        {
            dumpedNode.Flags |= DumpedTypeTreeNode::kNodeFlagIsArray;
        }

        IF_HAS_MEMBER(node, m_TypeFlags)
        {
            IF_HAS_ENUM_FLAG(node.m_TypeFlags, TypeTreeNode::kFlagIsArray, dumpedNode.Flags, DumpedTypeTreeNode::kNodeFlagIsArray);
            IF_HAS_ENUM_FLAG(node.m_TypeFlags, TypeTreeNode::kFlagIsManagedReference, dumpedNode.Flags, DumpedTypeTreeNode::kNodeFlagIsManagedReference);
            IF_HAS_ENUM_FLAG(node.m_TypeFlags, TypeTreeNode::kFlagIsManagedReferenceRegistry, dumpedNode.Flags, DumpedTypeTreeNode::kNodeFlagIsManagedReferenceRegistry);
            IF_HAS_ENUM_FLAG(node.m_TypeFlags, TypeTreeNode::kFlagIsArrayOfRefs, dumpedNode.Flags, DumpedTypeTreeNode::kNodeFlagIsArrayOfRefs);
        }

        IF_HAS_MEMBER(node, m_RefTypeHash)
        {
            dumpedNode.RefTypeHash = node.m_RefTypeHash;
        }
    }

    static void ConvertTransferInstructionFlags(const TransferInstructionFlags& flags, std::underlying_type_t<DumpedTransferInstructionFlags>& dumpedFlags)
    {
        // TODO: Add proper parsing for these
        IF_HAS_ENUM_FLAG(flags, TransferInstructionFlags::kSerializeGameRelease, dumpedFlags, DumpedTransferInstructionFlags::kTransferFlagSerializeGameRelease);
    }

#undef IF_HAS_MEMBER
#undef IF_HAS_MEMBER_PTR
#undef IF_HAS_ENUM_FLAG

public:
    void Add(const RTTI* rtti, const TypeTree& tree, const TransferInstructionFlags& flags, char const* commonStringBuffer)
    {
        DumpedTypeTree dumpedTree{};

        ConvertRTTI(rtti, dumpedTree.RTTI);

        ConvertTransferInstructionFlags(flags, dumpedTree.TransferFlags);

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
