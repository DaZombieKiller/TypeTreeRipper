#pragma once
#include "common.hpp"
#include "MemLabelId.hpp"
#include <string>

template<Revision R, Variant V>
class Object;

template<Revision R, Variant V>
struct RTTI;

DECLARE_REVISION(RTTI, Revision::V5_0);
DECLARE_REVISION(RTTI, Revision::V5_1);
DECLARE_REVISION(RTTI, Revision::V5_2);
DECLARE_REVISION(RTTI, Revision::V5_4);
DECLARE_REVISION(RTTI, Revision::V5_5);
DECLARE_REVISION(RTTI, Revision::V2017_3);

//
// 3.x & 4.x
//
enum ObjectCreationMode : int;

template<Revision R, Variant V>
struct RTTI
{
    RTTI *base;
    Object<R, V> *(*factory)(MemLabelId<R, V>, ObjectCreationMode);
    int32_t persistentTypeID;
    std::string className;
    int32_t size;
    bool isAbstract;
};

//
// 5.0
//

DEFINE_REVISION(struct, RTTI, Revision::V5_0)
{
    RTTI *base;
    Object<R, V> *(*factory)(MemLabelId<R, V>, ObjectCreationMode);
    int32_t persistentTypeID;
    std::string className;
    int32_t size;
    bool isAbstract;
    bool isSealed;
};

//
// 5.1
//

DEFINE_REVISION(struct, RTTI, Revision::V5_1)
{
    RTTI *base;
    Object<R, V> *(*factory)(MemLabelId<R, V>, ObjectCreationMode);
    int32_t persistentTypeID;
    std::string className;
    int32_t size;
    bool isAbstract;
    bool isSealed;
    bool isDeprecated;
};

//
// 5.2
//

DEFINE_REVISION(struct, RTTI, Revision::V5_2)
{
    RTTI *base;
    Object<R, V> *(*factory)(MemLabelId<R, V>, ObjectCreationMode);
    int32_t persistentTypeID;
    char const *className;
    int32_t size;
    bool isAbstract;
    bool isSealed;
    bool isDeprecated;
};

//
// 5.4
//

struct DerivedFromInfo
{
    uint32_t typeIndex;
    uint32_t descendantCount;
};

DEFINE_REVISION(struct, RTTI, Revision::V5_4)
{
    RTTI *base;
    Object<R, V> *(*factory)(MemLabelId<R, V>, ObjectCreationMode);
    int32_t persistentTypeID;
    char const *className;
    int32_t size;
    DerivedFromInfo derivedFromInfo;
    bool isAbstract;
    bool isSealed;
    bool isEditorOnly;
};

//
// 5.5
//

DEFINE_REVISION(struct, RTTI, Revision::V5_5)
{
    RTTI *base;
    Object<R, V> *(*factory)(MemLabelId<R, V>, ObjectCreationMode);
    char const *className;
    char const *classNamespace;
    int32_t persistentTypeID;
    int32_t size;
    DerivedFromInfo derivedFromInfo;
    bool isAbstract;
    bool isSealed;
    bool isEditorOnly;
};

//
// 2017.3
//

DEFINE_REVISION(struct, RTTI, Revision::V2017_3)
{
    RTTI *base;
    Object<R, V> *(*factory)(MemLabelId<R, V>, ObjectCreationMode);
    char const *className;
    char const *classNamespace;
    char const *module;
    int32_t persistentTypeID;
    int32_t size;
    DerivedFromInfo derivedFromInfo;
    bool isAbstract;
    bool isSealed;
    bool isEditorOnly;
    bool isStripped;
    class ConstVariantRef const *attributes;
    size_t attributeCount;
};

template<Revision R, Variant V>
struct RuntimeTypeArray
{
    int32_t Count;
    std::array<RTTI<R, V> *, 1024> Types;
};
