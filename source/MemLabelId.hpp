#pragma once
#include "common.hpp"
#include <cstdint>

//
// AllocationRootReference
//

struct AllocationRootReference;

//
// AllocationRootWithSalt
//

template<Revision R, Variant V>
struct AllocationRootWithSalt;

DECLARE_REVISION(AllocationRootWithSalt, Revision::V2023_1_0);

template<Revision R, Variant V>
struct AllocationRootWithSalt
{
    uint32_t m_Salt = 0;
    uint32_t m_RootReferenceIndex = 0;
};

DEFINE_REVISION(struct, AllocationRootWithSalt, Revision::V2023_1_0)
{
    uint16_t m_Salt = 0;
    uint32_t m_RootReferenceIndex = 0;
};

//
// MemLabelId
//

template<Revision R, Variant V>
struct MemLabelId;

DECLARE_REVISION_VARIANT(MemLabelId, Revision::V2017_1_0, Variant::RuntimeDev);
DECLARE_REVISION_VARIANT(MemLabelId, Revision::V2022_2_0, Variant::RuntimeDev);
DECLARE_REVISION(MemLabelId, Revision::V2023_1_0);

template<Revision R, Variant V>
struct MemLabelId
{
    uint32_t identifier = 0;
};

DEFINE_REVISION(struct, MemLabelId, Revision::V2023_1_0)
{
    uint16_t identifier = 0;
};

template<Revision R>
struct MemLabelId<R, Variant::RuntimeDev>
{
    uint32_t identifier = 0;
    AllocationRootReference *rootReference = nullptr;
};

DEFINE_REVISION_VARIANT(struct, MemLabelId, Revision::V2017_1_0, Variant::RuntimeDev)
{
    AllocationRootWithSalt<R, Variant::RuntimeDev> m_RootReferenceWithSalt;
    uint32_t identifier = 0;
};

DEFINE_REVISION_VARIANT(struct, MemLabelId, Revision::V2022_2_0, Variant::RuntimeDev)
{
    uint32_t identifier = 0;
    AllocationRootWithSalt<R, Variant::RuntimeDev> m_RootReferenceWithSalt;
};

DEFINE_REVISION_VARIANT(struct, MemLabelId, Revision::V2023_1_0, Variant::RuntimeDev)
{
    uint16_t identifier = 0;
    uint16_t salt = 0;
    uint32_t rootReferenceIndex = 0;
};

// Editor representation matches development player.
template<Revision R>
struct MemLabelId<R, Variant::Editor> : MemLabelId<R, Variant::RuntimeDev>
{
};
