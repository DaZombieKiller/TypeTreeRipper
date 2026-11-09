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
        m_ObjectPtr((char*)(objectPtr)),
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
    // i'm lazy and they refactored the entire thing :)
    // size grew by about 0x30 bytes
    uint8_t blob[0xc8];
public:
    GenerateTypeTreeTransfer(TypeTree<R, V> *t, TransferInstructionFlags<R, V> options, void *objectPtr, int32_t objectDataSize)
    {
        memset(blob, 0, sizeof(blob));

        *reinterpret_cast<int64_t *>((char*)(this) + 0x5c) = 0x4d;
        *reinterpret_cast<int32_t *>((char*)(this) + 0x64) = -1;
        *((char*)(this) + 0x50) = 0x18;
        *((char*)(this) + 0x58) = 1;
        *reinterpret_cast<int64_t **>((char*)(this) + 0x80) = reinterpret_cast<int64_t*>(t);
        *reinterpret_cast<void **>((char*)(this) + 8) = objectPtr;
        *reinterpret_cast<int64_t *>((char*)(this) + 0xa8) = objectDataSize;
        *reinterpret_cast<int64_t *>((char*)(this) + 0xb0) = options;
    }

    virtual int64_t GetCachePosition() {
        return -1;
    }

    virtual void Skip(uint64_t n) {
        *((char*)(this) + 0xcd) = 0;
    }
};
