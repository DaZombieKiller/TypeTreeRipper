#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <detours/detours.h>
#include <wil/win32_helpers.h>
#include <wil/win32_result_macros.h>
#include "common.hpp"
#include "dumper.hpp"
#include <filesystem>
#undef WIN32_LEAN_AND_MEAN

template<Revision R, Variant V>
class WindowsDumper
{
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

    static std::ofstream CreateOutputFile(char const *filename)
    {
        // Create the output file in the current directory.
        // NOTE: Should this be made configurable? i.e. through an environment variable?
        return std::ofstream(std::filesystem::current_path() / filename);
    }

    static void DebugLog(char const *message)
    {
        OutputDebugStringA(message);
        OutputDebugStringA("\n");
    }
private:
    std::vector<ExecutableSection> CachedSections;
};

//
// The dumper has two primary requirements:
//  1. It must run on the Unity main thread.
//  2. The engine needs to be initialized (so that object creation may succeed).
// To do this, we hook SetWindowLongPtrA (this is called right after engine init)
// to kick off the dumper and return a fail
// to cause the engine to quit afterwards.
//

namespace
{
    decltype(&SetWindowLongPtrA) pSetWindowLongPtrA;

    LONG_PTR WINAPI DetourSetWindowLongPtrA(HWND, int, LONG_PTR)
    {
        RunDumper<WindowsDumper>(Revision::V6000_0, Variant::Runtime);
        ExitProcess(0);
    }
}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, void *lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        FAIL_FAST_IF_WIN32_ERROR(DetourTransactionBegin());
        *(void **)&pSetWindowLongPtrA = GetProcAddress(GetModuleHandleW(L"USER32"), "SetWindowLongPtrA");
        FAIL_FAST_IF_WIN32_ERROR(DetourAttach((void **)&pSetWindowLongPtrA, &DetourSetWindowLongPtrA));
        FAIL_FAST_IF_WIN32_ERROR(DetourTransactionCommit());
    }

    return TRUE;
}
