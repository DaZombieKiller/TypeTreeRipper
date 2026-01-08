#pragma once
#include <cstdint>
#include "common.hpp"
#include "RTTI.hpp"
#include "GenerateTypeTreeTransfer.hpp"

enum ObjectCreationMode
{
    kCreateObjectDefault,
    kCreateObjectFromNonMainThread,
    kCreateObjectDefaultNoLock
};

template<Revision R, Variant V>
class Object;

DECLARE_REVISION(Object, Revision::V5_0);
DECLARE_REVISION(Object, Revision::V2017_1);
DECLARE_REVISION(Object, Revision::V2018_2);
DECLARE_REVISION(Object, Revision::V2021_1);

//
// 3.0 (default)
//

template<Revision R, Variant V>
class Object
{
    using ProxyTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    template<bool kSwap> class StreamedBinaryWrite;
    template<bool kSwap> class StreamedBinaryRead;
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(enum AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual void ClearPropertiesForCopySerialized() = 0;
    virtual bool ShouldIgnoreInGarbageDependencyTracking() = 0;
    virtual int32_t GetClassID() const = 0;
    virtual void SetHideFlags(int32_t) = 0;
    virtual int32_t GetRuntimeMemorySize() const = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(ProxyTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryWrite<true> &) = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryWrite<false> &) = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryRead<true> &) = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryRead<false> &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
};

//
// 5.0
//

DEFINE_REVISION(class, Object, Revision::V5_0)
{
    using ProxyTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    template<bool kSwap> class StreamedBinaryWrite;
    template<bool kSwap> class StreamedBinaryRead;
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(enum AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool ShouldIgnoreInGarbageDependencyTracking() = 0;
    virtual void SetHideFlags(int32_t) = 0;
    virtual int32_t GetRuntimeMemorySize() const = 0;
    virtual int32_t GetClassIDVirtualInternal() const = 0;
    virtual void SetCachedScriptingObject(struct MonoObject *) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryWrite<false> &) = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryRead<false> &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(ProxyTransfer &) = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryRead<true> &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryWrite<true> &) = 0;
};

//
// 2017.1
//

DEFINE_REVISION(class, Object, Revision::V2017_1)
{
    using RTTI = ::RTTI<R, V>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    template<bool kSwap> class StreamedBinaryWrite;
    template<bool kSwap> class StreamedBinaryRead;
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(enum AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool ShouldIgnoreInGarbageDependencyTracking() = 0;
    virtual void SetHideFlags(enum HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryWrite<false> &) = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryRead<false> &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryRead<true> &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(StreamedBinaryWrite<true> &) = 0;
};

//
// 2018.2
//

DEFINE_REVISION(class, Object, Revision::V2018_2)
{
    using RTTI = ::RTTI<R, V>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(enum AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool ShouldIgnoreInGarbageDependencyTracking() = 0;
    virtual void SetHideFlags(enum HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
};

//
// 2021.1
//

DEFINE_REVISION(class, Object, Revision::V2021_1)
{
    using RTTI = ::RTTI<R, V>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(enum AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual void SetHideFlags(enum HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
};
