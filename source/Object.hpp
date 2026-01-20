#pragma once
#include <cstdint>
#include "common.hpp"
#include "RTTI.hpp"
#include "GenerateTypeTreeTransfer.hpp"

enum ObjectCreationMode : int
{
    kCreateObjectDefault,
    kCreateObjectFromNonMainThread,
    kCreateObjectDefaultNoLock
};

template<Revision R, Variant V>
class Object;

DECLARE_REVISION(Object, Revision::V5_0_0);
DECLARE_REVISION_VARIANT(Object, Revision::V5_0_0, Variant::Editor);
DECLARE_REVISION(Object, Revision::V2017_1_0);
DECLARE_REVISION_VARIANT(Object, Revision::V2017_1_0, Variant::Editor);
DECLARE_REVISION_VARIANT(Object, Revision::V2017_4_0, Variant::Editor);
DECLARE_REVISION(Object, Revision::V2018_2_0);
DECLARE_REVISION_VARIANT(Object, Revision::V2018_2_0, Variant::Editor);
DECLARE_REVISION_VARIANT(Object, Revision::V2018_3_0, Variant::Editor);
DECLARE_REVISION(Object, Revision::V2021_1_0);
DECLARE_REVISION_VARIANT(Object, Revision::V2021_1_0, Variant::Editor);
DECLARE_REVISION_VARIANT(Object, Revision::V2022_1_0, Variant::Editor);
DECLARE_REVISION_VARIANT(Object, Revision::V2022_2_0, Variant::RuntimeDev);
DECLARE_REVISION_VARIANT(Object, Revision::V2022_2_0, Variant::Editor);
DECLARE_REVISION(Object, Revision::V6000_5_0);
DECLARE_REVISION_VARIANT(Object, Revision::V6000_5_0, Variant::RuntimeDev);
DECLARE_REVISION_VARIANT(Object, Revision::V6000_5_0, Variant::Editor);

//
// 3.0 (default)
//

enum AwakeFromLoadMode : int;

template<Revision R, Variant V>
class Object
{
    using ProxyTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    template<bool kSwap> class StreamedBinaryWrite;
    template<bool kSwap> class StreamedBinaryRead;
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
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

// TODO: Editor

//
// 5.0
//

DEFINE_REVISION(class, Object, Revision::V5_0_0)
{
    using ProxyTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    template<bool kSwap> class StreamedBinaryWrite;
    template<bool kSwap> class StreamedBinaryRead;
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
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

DEFINE_REVISION_VARIANT(class, Object, Revision::V5_0_0, Variant::Editor)
{
    using ProxyTransfer = ::GenerateTypeTreeTransfer<R, Variant::Editor>;
public:
    template<bool kSwap> class StreamedBinaryWrite;
    template<bool kSwap> class StreamedBinaryRead;
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool HasDebugmodeAutoRefreshInspector() = 0;
    virtual void WarnInstantiateDisallowed() = 0;
    virtual void CloneAdditionalEditorProperties(Object const &) = 0;
    virtual bool CanAssignMonoVariable(char const *, Object *) = 0;
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
    virtual void VirtualRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead<false> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead<true> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite<false> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite<true> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(ProxyTransfer &) = 0;
};

//
// 2017.1
//

enum HideFlags : int;

DEFINE_REVISION(class, Object, Revision::V2017_1_0)
{
    using RTTI = ::RTTI<R, V>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    template<bool kSwap> class StreamedBinaryWrite;
    template<bool kSwap> class StreamedBinaryRead;
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool ShouldIgnoreInGarbageDependencyTracking() = 0;
    virtual void SetHideFlags(HideFlags) = 0;
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

DEFINE_REVISION_VARIANT(class, Object, Revision::V2017_1_0, Variant::Editor)
{
    using RTTI = ::RTTI<R, Variant::Editor>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, Variant::Editor>;
public:
    template<bool kSwap> class StreamedBinaryWrite;
    template<bool kSwap> class StreamedBinaryRead;
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool HasDebugmodeAutoRefreshInspector() = 0;
    virtual void WarnInstantiateDisallowed() = 0;
    virtual void CloneAdditionalEditorProperties(Object const &) = 0;
    virtual bool CanAssignMonoVariable(char const *, Object *) = 0;
    virtual bool ShouldIgnoreInGarbageDependencyTracking() = 0;
    virtual void SetHideFlags(HideFlags) = 0;
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
    virtual void VirtualRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead<false> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead<true> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite<false> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite<true> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
};

//
// 2017.4
//

DEFINE_REVISION_VARIANT(class, Object, Revision::V2017_4_0, Variant::Editor)
{
    using RTTI = ::RTTI<R, Variant::Editor>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, Variant::Editor>;
public:
    template<bool kSwap> class StreamedBinaryWrite;
    template<bool kSwap> class StreamedBinaryRead;
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool HasDebugmodeAutoRefreshInspector() = 0;
    virtual void WarnInstantiateDisallowed() = 0;
    virtual void CloneAdditionalEditorProperties(Object const &) = 0;
    virtual bool CanAssignMonoVariable(char const *, Object *) = 0;
    virtual bool ShouldIgnoreInGarbageDependencyTracking() = 0;
    virtual void SetPersistentDirtyIndex(uint32_t) = 0;
    virtual uint32_t GetPersistentDirtyIndex() = 0;
    virtual void ClearPersistentDirty() = 0;
    virtual void SetHideFlags(HideFlags) = 0;
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
    virtual void VirtualRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead<false> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead<true> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite<false> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite<true> &) = 0;
    virtual void VirtualStrippedRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
};

//
// 2018.2
//

DEFINE_REVISION(class, Object, Revision::V2018_2_0)
{
    using RTTI = ::RTTI<R, V>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool ShouldIgnoreInGarbageDependencyTracking() = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
};

DEFINE_REVISION_VARIANT(class, Object, Revision::V2018_2_0, Variant::Editor)
{
    using RTTI = ::RTTI<R, Variant::Editor>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, Variant::Editor>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool HasDebugmodeAutoRefreshInspector() = 0;
    virtual void WarnInstantiateDisallowed() = 0;
    virtual void CloneAdditionalEditorProperties(Object const &) = 0;
    virtual bool CanAssignMonoVariable(char const *, Object *) = 0;
    virtual bool ShouldIgnoreInGarbageDependencyTracking() = 0;
    virtual void SetPersistentDirtyIndex(uint32_t) = 0;
    virtual uint32_t GetPersistentDirtyIndex() = 0;
    virtual void ClearPersistentDirty() = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
};

//
// 2018.3
//

DEFINE_REVISION_VARIANT(class, Object, Revision::V2018_3_0, Variant::Editor)
{
    using RTTI = ::RTTI<R, Variant::Editor>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, Variant::Editor>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool HasDebugmodeAutoRefreshInspector() = 0;
    virtual void WarnInstantiateDisallowed() = 0;
    virtual void CloneAdditionalEditorProperties(Object const &) = 0;
    virtual bool CanAssignMonoVariable(char const *, Object *) = 0;
    virtual bool ShouldIgnoreInGarbageDependencyTracking() = 0;
    virtual void SetPersistentDirtyIndex(uint32_t) = 0;
    virtual uint32_t GetPersistentDirtyIndex() = 0;
    virtual void ClearPersistentDirty() = 0;
    virtual void SetIsPreviewSceneObject(bool) = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
};

//
// 2021.1
//

DEFINE_REVISION(class, Object, Revision::V2021_1_0)
{
    using RTTI = ::RTTI<R, V>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
};

DEFINE_REVISION_VARIANT(class, Object, Revision::V2021_1_0, Variant::Editor)
{
    using RTTI = ::RTTI<R, Variant::Editor>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, Variant::Editor>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool HasDebugmodeAutoRefreshInspector() = 0;
    virtual void WarnInstantiateDisallowed() = 0;
    virtual void CloneAdditionalEditorProperties(Object const &) = 0;
    virtual bool CanAssignMonoVariable(char const *, Object *) = 0;
    virtual void SetPersistentDirtyIndex(uint32_t) = 0;
    virtual uint32_t GetPersistentDirtyIndex() = 0;
    virtual void ClearPersistentDirty() = 0;
    virtual void SetIsPreviewSceneObject(bool) = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
};

//
// 2022.1
//

enum PreviewSceneFlags : int;

DEFINE_REVISION_VARIANT(class, Object, Revision::V2022_1_0, Variant::Editor)
{
    using RTTI = ::RTTI<R, Variant::Editor>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, Variant::Editor>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool HasDebugmodeAutoRefreshInspector() = 0;
    virtual void WarnInstantiateDisallowed() = 0;
    virtual void CloneAdditionalEditorProperties(Object const &) = 0;
    virtual bool CanAssignMonoVariable(char const *, Object *) = 0;
    virtual void SetPersistentDirtyIndex(uint32_t) = 0;
    virtual uint32_t GetPersistentDirtyIndex() = 0;
    virtual void ClearPersistentDirty() = 0;
    virtual void SetPreviewSceneFlags(PreviewSceneFlags) = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
};

//
// 2022.2
//

// Added 'GetMetaData'
// TODO: Determine exact version this was added in

DEFINE_REVISION_VARIANT(class, Object, Revision::V2022_2_0, Variant::RuntimeDev)
{
    using RTTI = ::RTTI<R, Variant::RuntimeDev>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, Variant::RuntimeDev>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual bool GetMetaData(class GrowableBuffer &) = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
};

DEFINE_REVISION_VARIANT(class, Object, Revision::V2022_2_0, Variant::Editor)
{
    using RTTI = ::RTTI<R, Variant::Editor>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, Variant::Editor>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool HasDebugmodeAutoRefreshInspector() = 0;
    virtual void WarnInstantiateDisallowed() = 0;
    virtual void CloneAdditionalEditorProperties(Object const &) = 0;
    virtual bool CanAssignMonoVariable(char const *, Object *) = 0;
    virtual void SetPersistentDirtyIndex(uint32_t) = 0;
    virtual uint32_t GetPersistentDirtyIndex() = 0;
    virtual void ClearPersistentDirty() = 0;
    virtual void SetPreviewSceneFlags(PreviewSceneFlags) = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual bool GetMetaData(class GrowableBuffer &) = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
};

//
// 6000.5
//

DEFINE_REVISION(class, Object, Revision::V6000_5_0)
{
    using RTTI = ::RTTI<R, V>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, V>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class UDMReadTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class UDMWriteTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
};

DEFINE_REVISION_VARIANT(class, Object, Revision::V6000_5_0, Variant::RuntimeDev)
{
    using RTTI = ::RTTI<R, Variant::RuntimeDev>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, Variant::RuntimeDev>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual bool GetMetaData(class GrowableBuffer &) = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class UDMReadTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class UDMWriteTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
};

DEFINE_REVISION_VARIANT(class, Object, Revision::V6000_5_0, Variant::Editor)
{
    using RTTI = ::RTTI<R, Variant::Editor>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, Variant::Editor>;
public:
    virtual ~Object() = 0;
    virtual void MainThreadCleanup() = 0;
    virtual void AwakeFromLoad(AwakeFromLoadMode) = 0;
    virtual void AwakeFromLoadThreaded() = 0;
    virtual void CheckConsistency() = 0;
    virtual void Reset() = 0;
    virtual void SmartReset() = 0;
    virtual bool IsAScriptedObject() = 0;
    virtual RTTI const *GetTypeVirtualInternal() = 0;
    virtual char const *GetName() const = 0;
    virtual void SetName(char const *) = 0;
    virtual bool HasDebugmodeAutoRefreshInspector() = 0;
    virtual void WarnInstantiateDisallowed() = 0;
    virtual void CloneAdditionalEditorProperties(Object const &) = 0;
    virtual bool CanAssignMonoVariable(char const *, Object *) = 0;
    virtual void SetPersistentDirtyIndex(uint32_t) = 0;
    virtual uint32_t GetPersistentDirtyIndex() = 0;
    virtual void ClearPersistentDirty() = 0;
    virtual void SetPreviewSceneFlags(PreviewSceneFlags) = 0;
    virtual void SetHideFlags(HideFlags) = 0;
    virtual size_t GetRuntimeMemorySize() const = 0;
    virtual bool GetMetaData(class GrowableBuffer &) = 0;
    virtual void SetCachedScriptingObject(class ScriptingObjectPtr) = 0;
    virtual bool GetNeedsPerObjectTypeTree() const = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class RemapPPtrTransfer &) = 0;
    virtual void VirtualRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class UDMReadTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class UDMWriteTransfer &) = 0;
    virtual void VirtualRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualRedirectTransfer(class UDMGenerateSchemaTransfer &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class SafeBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class YAMLWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class JSONWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class UDMWriteTransfer &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryRead &) = 0;
    virtual void VirtualStrippedRedirectTransfer(class StreamedBinaryWrite &) = 0;
    virtual void VirtualStrippedRedirectTransfer(GenerateTypeTreeTransfer &) = 0;
};
