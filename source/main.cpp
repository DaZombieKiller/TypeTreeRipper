#define WIN32_LEAN_AND_MEAN
#include <array>
#include <fstream>
#include "common.hpp"
#include "MemLabelId.hpp"
#include "Object.hpp"
#include "GenerateTypeTreeTransfer.hpp"
#include <Windows.h>
#include <detours/detours.h>
#include <wil/win32_helpers.h>
#include <wil/win32_result_macros.h>

struct IDumper
{
    virtual void Run() = 0;
};

static HMODULE GetUnityModule()
{
    auto hModule = GetModuleHandleW(L"UnityPlayer.dll");

    if (hModule == nullptr)
        hModule = GetModuleHandleW(L"Unity.dll");

    if (hModule == nullptr)
        hModule = GetModuleHandleW(L"Unity.exe");

    return hModule;
}

template<Revision R, Variant V>
class Dumper : public IDumper
{
    using Object = ::Object<R, V>;
    using TypeTree = ::TypeTree<R, V>;
    using TypeTreeShareableData = ::TypeTreeShareableData<R, V>;
    using TransferInstructionFlags = ::TransferInstructionFlags<R, V>;
    using MemLabelId = ::MemLabelId<R, V>;
    using GenerateTypeTreeTransfer = ::GenerateTypeTreeTransfer<R, V>;
    using RuntimeTypeArray = ::RuntimeTypeArray<R, V>;
    using RTTI = ::RTTI<R, V>;

    bool StringEquals(char const *p, std::string_view other)
    {
        if (IsBadReadPtr(p, other.size() + 1))
            return false;

        return !memcmp(p, other.data(), other.size()) && p[other.size()] == '\0';
    }

    bool IsValidRuntimeTypeArray(RuntimeTypeArray const *pArray)
    {
        if (IsBadReadPtr(pArray, sizeof(RuntimeTypeArray)))
            return false;

        if constexpr (R >= Revision::V5_2)
        {
            if (pArray->Count < 2 || pArray->Count > ARRAYSIZE(pArray->Types))
                return false;

            for (int i = 0; i < pArray->Count; i++)
            {
                if (IsBadReadPtr(pArray->Types[i], sizeof(RTTI)))
                {
                    return false;
                }

                if (pArray->Types[i]->factory && IsBadCodePtr((FARPROC)pArray->Types[i]->factory))
                {
                    return false;
                }
            }

            if (pArray->Types[0]->persistentTypeID != 0)
                return false;

            if (pArray->Types[1]->base != pArray->Types[0])
                return false;

            if (!StringEquals(pArray->Types[0]->className, "Object"))
                return false;

            return true;
        }

        return false;
    }

    RuntimeTypeArray const *GetRuntimeTypeArray()
    {
        auto pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER *>(GetUnityModule());
        auto pNtHeaders = reinterpret_cast<IMAGE_NT_HEADERS *>(reinterpret_cast<char *>(pDosHeader) + pDosHeader->e_lfanew);

        for (WORD iSection = 0; iSection < pNtHeaders->FileHeader.NumberOfSections; iSection++)
        {
            auto pSection = IMAGE_FIRST_SECTION(pNtHeaders) + iSection;
            auto pContent = reinterpret_cast<char *>(pDosHeader) + pSection->VirtualAddress;

            // The runtime type array is initialized at runtime, if the section
            // is not writable then it can not contain the runtime type array.
            if ((pSection->Characteristics & IMAGE_SCN_MEM_WRITE) == 0)
                continue;

            if ((pSection->Characteristics & IMAGE_SCN_MEM_READ) == 0)
                continue;

            for (size_t i = 0; i < pSection->Misc.VirtualSize - sizeof(RuntimeTypeArray); i++)
            {
                auto pArray = reinterpret_cast<RuntimeTypeArray const *>(pContent + i);

                if (!IsValidRuntimeTypeArray(pArray))
                    continue;

                return pArray;
            }
        }

        return nullptr;
    }

    char const *GetCommonStringBuffer()
    {
        auto pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER *>(GetUnityModule());
        auto pNtHeaders = reinterpret_cast<IMAGE_NT_HEADERS *>(reinterpret_cast<char *>(pDosHeader) + pDosHeader->e_lfanew);
        auto searchItem = std::string_view("AABB\0AnimationClip\0", 19);

        for (WORD iSection = 0; iSection < pNtHeaders->FileHeader.NumberOfSections; iSection++)
        {
            auto pSection = IMAGE_FIRST_SECTION(pNtHeaders) + iSection;
            auto pContent = reinterpret_cast<char *>(pDosHeader) + pSection->VirtualAddress;

            if ((pSection->Characteristics & IMAGE_SCN_MEM_READ) == 0)
                continue;

            if ((pSection->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) != 0)
                continue;

            for (size_t i = 0; i < pSection->Misc.VirtualSize - searchItem.size(); i++)
            {
                if (!memcmp(pContent + i, searchItem.data(), searchItem.size()))
                {
                    return pContent + i;
                }
            }
        }

        return nullptr;
    }

    void DumpNodes(TypeTreeShareableData &data, char const *commonString, std::ofstream &ofs)
    {
        int minLevel = INT_MAX;

        for (int n = 0; n < data.Nodes().size(); n++)
        {
            auto &node = data.Nodes()[n];

            if (node.m_Level < minLevel)
            {
                minLevel = node.m_Level;
            }
        }

        for (int n = 0; n < data.Nodes().size(); n++)
        {
            auto &node = data.Nodes()[n];

            for (int ind = 0; ind < node.m_Level - minLevel; ind++)
                ofs << '\t';

            if ((node.m_TypeStrOffset & 0x80000000) == 0)
            {
                ofs << data.StringsBuffer().data() + node.m_TypeStrOffset;
            }
            else
            {
                ofs << commonString + (node.m_TypeStrOffset & ~0x80000000);
            }

            ofs << " ";

            if ((node.m_NameStrOffset & 0x80000000) == 0)
            {
                ofs << data.StringsBuffer().data() + node.m_NameStrOffset;
            }
            else
            {
                ofs << commonString + (node.m_NameStrOffset & ~0x80000000);
            }

            ofs.put(ofs.widen('\n'));
        }
    }
public:
    void Run() override
    {
        if constexpr (R >= Revision::V5_2)
        {
            auto pArray = GetRuntimeTypeArray();
            auto pTable = GetCommonStringBuffer();
            std::ofstream ofs("types.txt");

            for (int i = 0; i < pArray->Count; i++)
            {
                RTTI *pRTTI = pArray->Types[i];

                if (pRTTI->isAbstract || !pRTTI->factory)
                    continue;

                MemLabelId label;
                Object *object = pRTTI->factory(label, kCreateObjectDefault);
                TypeTreeShareableData data(label);
                TypeTree tree(&data, label);
                GenerateTypeTreeTransfer transfer(tree, TransferInstructionFlags::kSerializeGameRelease, object, pRTTI->size);
                object->VirtualRedirectTransfer(transfer);
                DumpNodes(data, pTable, ofs);
            }
        }
    }

    static Dumper<R, V> Instance;
};

template<Revision R, Variant V>
Dumper<R, V> Dumper<R, V>::Instance;

template<Variant V>
struct DumperArray
{
    static inline constexpr auto Instances = []<size_t... I>(std::index_sequence<I...>)
    {
        return std::array<IDumper *, sizeof...(I)>{ &Dumper<static_cast<Revision>(I), V>::Instance... };
    }(std::make_index_sequence<static_cast<size_t>(Revision::Count)>{});
};

static void RunDumper(Revision revision, Variant variant)
{
    switch (variant)
    {
    case Variant::Editor:
        DumperArray<Variant::Editor>::Instances[(int)revision]->Run();
        break;
    case Variant::Runtime:
        DumperArray<Variant::Runtime>::Instances[(int)revision]->Run();
        break;
    case Variant::RuntimeDev:
        DumperArray<Variant::RuntimeDev>::Instances[(int)revision]->Run();
        break;
    }
}

//
// The dumper has two primary requirements:
//  1. It must run on the Unity main thread.
//  2. The engine needs to be initialized (so that object creation may succeed).
// To do this, we hook LoadLibrary and wait for Mono to be loaded, then we hook
// mono_image_open_from_data_with_name to kick off the dumper and return a fail
// to cause the engine to quit afterwards.
//

decltype(&LoadLibraryA) pLoadLibraryA;

decltype(&LoadLibraryW) pLoadLibraryW;

wil::critical_section MonoLock;

bool MonoDetoured;

void *(*pMonoImageOpen)(char *, uint32_t, int32_t, void *, int32_t, char const *);

void *DetourMonoImageOpen(char *, uint32_t, int32_t, void *, int32_t, char const *)
{
    // TODO: Determine which variant and revision to use
    RunDumper(Revision::V6000_0, Variant::Runtime);
    return nullptr;
}

static bool IsMonoModule(HMODULE module)
{
    if (module == nullptr)
        return false;

    return module == GetModuleHandleW(L"mono-2.0-bdwgc.dll")
        || module == GetModuleHandleW(L"mono.dll");
}

LONG DetourMono(HMODULE hMono)
{
    auto lock = MonoLock.lock();

    if (MonoDetoured)
        return ERROR_SUCCESS;

    RETURN_IF_WIN32_ERROR(DetourTransactionBegin());
    *(void **)&pMonoImageOpen = GetProcAddress(hMono, "mono_image_open_from_data_with_name");
    RETURN_IF_WIN32_ERROR(DetourAttach((void **)&pMonoImageOpen, &DetourMonoImageOpen));
    RETURN_IF_WIN32_ERROR(DetourTransactionCommit());
    MonoDetoured = true;
    return ERROR_SUCCESS;
}

static HMODULE WINAPI DetourLoadLibraryW(LPCWSTR lpLibFileName)
{
    HMODULE hModule = pLoadLibraryW(lpLibFileName);

    if (IsMonoModule(hModule))
    {
        FAIL_FAST_IF_WIN32_ERROR(DetourMono(hModule));
    }

    return hModule;
}

static HMODULE WINAPI DetourLoadLibraryA(LPCSTR lpLibFileName)
{
    HMODULE hModule = pLoadLibraryA(lpLibFileName);

    if (IsMonoModule(hModule))
    {
        FAIL_FAST_IF_WIN32_ERROR(DetourMono(hModule));
    }

    return hModule;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, void *lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        FAIL_FAST_IF_WIN32_ERROR(DetourTransactionBegin());
        *(void **)&pLoadLibraryW = GetProcAddress(GetModuleHandleW(L"KERNEL32"), "LoadLibraryW");
        *(void **)&pLoadLibraryA = GetProcAddress(GetModuleHandleW(L"KERNEL32"), "LoadLibraryA");
        FAIL_FAST_IF_WIN32_ERROR(DetourAttach((void **)&pLoadLibraryW, &DetourLoadLibraryW));
        FAIL_FAST_IF_WIN32_ERROR(DetourAttach((void **)&pLoadLibraryA, &DetourLoadLibraryA));
        FAIL_FAST_IF_WIN32_ERROR(DetourTransactionCommit());
    }

    return TRUE;
}
