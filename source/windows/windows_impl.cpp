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

public:
    std::span<ExecutableSection> GetExecutableSections()
    {
        if (CachedSections.empty())
        {
            auto pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER *>(GetUnityModule());
            auto pNtHeaders = reinterpret_cast<IMAGE_NT_HEADERS *>(reinterpret_cast<char *>(pDosHeader) + pDosHeader->e_lfanew);

            for (WORD iSection = 0; iSection < pNtHeaders->FileHeader.NumberOfSections; iSection++)
            {
                auto pSection = IMAGE_FIRST_SECTION(pNtHeaders) + iSection;
                auto pContent = reinterpret_cast<char *>(pDosHeader) + pSection->VirtualAddress;

                // Ignore all discardable sections
                if ((pSection->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) != 0)
                    continue;

                auto protection = 0;
                if ((pSection->Characteristics & IMAGE_SCN_MEM_READ) != 0)
                    protection |= ExecutableSection::kSectionProtectionRead;

                if ((pSection->Characteristics & IMAGE_SCN_MEM_WRITE) != 0)
                    protection |= ExecutableSection::kSectionProtectionWrite;

                if ((pSection->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0)
                    protection |= ExecutableSection::kSectionProtectionExecute;

                const auto data = std::span(pContent, pSection->Misc.VirtualSize);
                CachedSections.emplace_back(data, protection);
            }
        }

        return CachedSections;
    }
private:
    std::vector<ExecutableSection> CachedSections;
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
