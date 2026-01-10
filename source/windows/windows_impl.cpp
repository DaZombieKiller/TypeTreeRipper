#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <detours/detours.h>
#include <wil/win32_helpers.h>
#include <wil/win32_result_macros.h>
#include "common.hpp"
#include "dumper.hpp"
#include "RTTI.hpp"
#include <span>
#include <functional>
#include <algorithm>
#undef WIN32_LEAN_AND_MEAN

template<Revision R, Variant V>
class WindowsDumper
{
    using RuntimeTypeArray = ::RuntimeTypeArray<R, V>;
    using RTTI = ::RTTI<R, V>;

    static HMODULE GetUnityModule()
    {
        for (const auto &module : {
            L"UnityPlayer.dll",
            L"Unity.dll", 
            L"Unity.exe" 
        })
        {
            const auto hModule = GetModuleHandleW(module);
            if (hModule != nullptr)
                return hModule;
        }

        FAIL_FAST_WIN32(ERROR_NOT_FOUND);
    }

    static bool StringEquals(char const *p, const std::string_view other)
    {
        if (IsBadReadPtr(p, other.size() + 1))
            return false;

        return p[other.size()] == '\0' && std::string_view(p, other.size()) == other;
    }

    static bool IsValidRuntimeTypeArray(RuntimeTypeArray const *pArray)
    {
        if (IsBadReadPtr(pArray, sizeof(RuntimeTypeArray)))
            return false;

        if constexpr (R >= Revision::V5_2)
        {
            if (pArray->Count < 2 || pArray->Count > pArray->Types.size())
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

public:
    static RuntimeTypeArray const *GetRuntimeTypeArray()
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

    static char const *GetCommonStringBuffer()
    {
        static constexpr auto kCommonStringBufferPattern = std::span("AABB\0AnimationClip");
        const auto searcher = std::boyer_moore_horspool_searcher(kCommonStringBufferPattern.cbegin(), kCommonStringBufferPattern.cend());

        auto pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER *>(GetUnityModule());
        auto pNtHeaders = reinterpret_cast<IMAGE_NT_HEADERS *>(reinterpret_cast<char *>(pDosHeader) + pDosHeader->e_lfanew);

        for (WORD iSection = 0; iSection < pNtHeaders->FileHeader.NumberOfSections; iSection++)
        {
            auto pSection = IMAGE_FIRST_SECTION(pNtHeaders) + iSection;
            auto pContent = reinterpret_cast<char *>(pDosHeader) + pSection->VirtualAddress;

            if ((pSection->Characteristics & IMAGE_SCN_MEM_READ) == 0)
                continue;

            if ((pSection->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) != 0)
                continue;

            const auto region = std::span(pContent, pSection->Misc.VirtualSize);
            if (const auto result = std::search(region.cbegin(), region.cend(), searcher);
                result != region.end())
            {
                return pContent + std::distance(region.cbegin(), result);
            }
        }

        return nullptr;
    }
};

//
// The dumper has two primary requirements:
//  1. It must run on the Unity main thread.
//  2. The engine needs to be initialized (so that object creation may succeed).
// To do this, we hook LoadLibrary and wait for Mono to be loaded, then we hook
// mono_image_open_from_data_with_name to kick off the dumper and return a fail
// to cause the engine to quit afterwards.
//

namespace
{
    decltype(&LoadLibraryA) pLoadLibraryA;

    decltype(&LoadLibraryW) pLoadLibraryW;

    wil::critical_section MonoLock;

    bool MonoDetoured;

    void *(*pMonoImageOpen)(char *, uint32_t, int32_t, void *, int32_t, char const *);
    void *DetourMonoImageOpen(char *, uint32_t, int32_t, void *, int32_t, char const *)
    {
        // TODO: Determine which variant and revision to use
        RunDumper<WindowsDumper>(Revision::V6000_0, Variant::Runtime);
        return nullptr;
    }

    bool IsMonoModule(HMODULE module)
    {
        if (module == nullptr)
            return false;

        return module == GetModuleHandleW(L"mono-2.0-bdwgc.dll")
            || module == GetModuleHandleW(L"mono.dll");
    }

    LONG DetourMono(HMODULE hMono)
    {
        auto _ = MonoLock.lock();

        if (MonoDetoured)
            return ERROR_SUCCESS;

        RETURN_IF_WIN32_ERROR(DetourTransactionBegin());
        *(void **)&pMonoImageOpen = GetProcAddress(hMono, "mono_image_open_from_data_with_name");
        RETURN_IF_WIN32_ERROR(DetourAttach((void **)&pMonoImageOpen, &DetourMonoImageOpen));
        RETURN_IF_WIN32_ERROR(DetourTransactionCommit());
        MonoDetoured = true;
        return ERROR_SUCCESS;
    }

    HMODULE WINAPI DetourLoadLibraryW(LPCWSTR lpLibFileName)
    {
        HMODULE hModule = pLoadLibraryW(lpLibFileName);

        if (IsMonoModule(hModule))
        {
            FAIL_FAST_IF_WIN32_ERROR(DetourMono(hModule));
        }

        return hModule;
    }

    HMODULE WINAPI DetourLoadLibraryA(LPCSTR lpLibFileName)
    {
        HMODULE hModule = pLoadLibraryA(lpLibFileName);

        if (IsMonoModule(hModule))
        {
            FAIL_FAST_IF_WIN32_ERROR(DetourMono(hModule));
        }

        return hModule;
    }
}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, void *lpvReserved)
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
