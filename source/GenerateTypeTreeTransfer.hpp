#pragma once
#include "common.hpp"
#include "TransferBase.hpp"
#include "TypeTree.hpp"

template<Revision R, Variant V>
class GenerateTypeTreeTransfer;

DECLARE_REVISION(GenerateTypeTreeTransfer, Revision::V5_0_0);

//
// 4.x
//

template<Revision R, Variant V>
class GenerateTypeTreeTransfer : public TransferBase<R, V>
{
    TypeTree<R, V> &m_TypeTree;
    TypeTree<R, V>* m_ActiveFather;
    char *m_ObjectPtr;
    int32_t m_ObjectSize;
    int32_t m_Index = 0;
    int32_t m_SimulatedByteOffset = 0;
    bool m_DidErrorAlignment = false;
    bool m_RequireTypelessData = false;
public:
    GenerateTypeTreeTransfer(TypeTree<R, V> &t, TransferInstructionFlags<R, V> options, void *objectPtr, int32_t objectDataSize) :
        m_TypeTree(t),
        m_ActiveFather(nullptr),
        m_ObjectPtr(static_cast<char *>(objectPtr)),
        m_ObjectSize(objectDataSize)
    {
        this->m_Flags = options;

        if (t.GetData())
        {
            t.GetData()->SetGenerationFlags(options);
        }
    }
};

//
// 5.0 to current
//

DEFINE_REVISION(class, GenerateTypeTreeTransfer, Revision::V5_0_0) : public TransferBase<R, V>
{
    TypeTree<R, V> &m_TypeTree;
    TypeTreeIterator<R, V> m_ActiveFather;
    char *m_ObjectPtr;
    int32_t m_ObjectSize;
    char *m_ScriptingObjectPtr = nullptr;
    int32_t m_ScriptingObjectSize = 0;
    int32_t m_Index = 0;
    int32_t m_SimulatedByteOffset = 0;
    bool m_DidErrorAlignment = false;
    bool m_RequireTypelessData = false;
public:
    GenerateTypeTreeTransfer(TypeTree<R, V> &t, TransferInstructionFlags<R, V> options, void *objectPtr, int32_t objectDataSize) :
        m_TypeTree(t),
        m_ActiveFather(nullptr, nullptr, 0),
        m_ObjectPtr(static_cast<char *>(objectPtr)),
        m_ObjectSize(objectDataSize)
    {
        this->m_Flags = options;

        if (t.GetData())
        {
            t.GetData()->SetGenerationFlags(options);
        }
    }
};
