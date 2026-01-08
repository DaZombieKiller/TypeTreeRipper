#pragma once
#include "common.hpp"
#include "TypeTree.hpp"

template<Revision R, Variant V>
class TransferBase;

DECLARE_REVISION(TransferBase, Revision::V5_0);
DECLARE_REVISION(TransferBase, Revision::V2019_1);
DECLARE_REVISION(TransferBase, Revision::V2019_3);
DECLARE_REVISION(TransferBase, Revision::V2019_4);
DECLARE_REVISION(TransferBase, Revision::V2021_1);
DECLARE_REVISION(TransferBase, Revision::V2022_3);

//
// 4.x
//

template<Revision R, Variant V>
class TransferBase
{
protected:
    TransferInstructionFlags<R, V> m_Flags = {};
    void *m_UserData = nullptr;
};

//
// 5.0
//

DEFINE_REVISION(class, TransferBase, Revision::V5_0)
{
protected:
    TransferInstructionFlags<R, V> m_Flags = {};
    void *m_UserData = nullptr;
    MemLabelId<R, V> m_MemLabel;
};

//
// 2019.1
//

DEFINE_REVISION(class, TransferBase, Revision::V2019_1)
{
protected:
    TransferInstructionFlags<R, V> m_Flags = {};
    void *m_UserData = nullptr;
    MemLabelId<R, V> m_MemLabel;
    void *m_ExtraDataForSerializingManagedReferences = nullptr;
};

//
// 2019.3
//

DEFINE_REVISION(class, TransferBase, Revision::V2019_3)
{
protected:
    TransferInstructionFlags<R, V> m_Flags = {};
    void *m_UserData = nullptr;
    MemLabelId<R, V> m_MemLabel;
    void *m_ExtraDataForSerializingManagedReferences = nullptr;
    bool m_MissingManagedTypes = false;
};

//
// 2019.4
//

DEFINE_REVISION(class, TransferBase, Revision::V2019_4)
{
protected:
    TransferInstructionFlags<R, V> m_Flags = {};
    void *m_UserData = nullptr;
    void *m_ExtraDataForSerializingManagedReferences = nullptr;
    void *m_ReferenceFromIDCache = nullptr;
    bool m_MissingManagedTypes = false;
};

//
// 2021.1
//

DEFINE_REVISION(class, TransferBase, Revision::V2021_1)
{
protected:
    TransferInstructionFlags<R, V> m_Flags = {};
    void *m_UserData = nullptr;
    void *m_ManagedReferencesTransferState = nullptr;
    void *m_GetManagedReferencesRegistry = nullptr;
    bool m_MissingManagedTypes = false;
};

//
// 2022.3
//

DEFINE_REVISION(class, TransferBase, Revision::V2022_3)
{
protected:
    TransferInstructionFlags<R, V> m_Flags = {};
    void *m_UserData = nullptr;
    void *m_ManagedReferencesTransferState = nullptr;
    void *m_GetManagedReferencesRegistry = nullptr;
    void *m_CustomLSOIResolver = nullptr;
    char const *m_ResourceImagePathPrefix = nullptr;
    bool m_MissingManagedTypes = false;
};
