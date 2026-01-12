#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <detours/detours.h>
#include <wil/win32_helpers.h>
#include <wil/win32_result_macros.h>
#include "common.hpp"
#include "dumper.hpp"
#include <filesystem>

#pragma comment(lib, "Version.lib")
#undef WIN32_LEAN_AND_MEAN

namespace
{
    HMODULE GetUnityModule()
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
}

template<Revision R, Variant V>
class WindowsDumper
{
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
    constexpr auto kForceRevisionEnvironmentVariable = L"TYPETREERIPPER_FORCE_REVISION";
    constexpr auto kForceVariantEnvironmentVariable = L"TYPETREERIPPER_FORCE_VARIANT";

    std::optional<Revision> GetUnityRevisionFromExecutable()
    {
        const auto module = GetUnityModule();
        const auto unityModuleFilepath = wil::GetModuleFileNameW(module);

        if (const auto size = GetFileVersionInfoSizeW(unityModuleFilepath.get(), nullptr);
            size > 0)
        {
            const auto versionData = std::make_unique<char[]>(size);
            if (GetFileVersionInfoW(unityModuleFilepath.get(), NULL, size, versionData.get()))
            {
                void *versionBuffer = nullptr;
                auto versionBufferSize = 0u;

                if (VerQueryValueW(versionData.get(), L"\\", &versionBuffer, &versionBufferSize)
                    && versionBuffer)
                {
                    if (const auto versionInfo = static_cast<VS_FIXEDFILEINFO *>(versionBuffer);
                        versionInfo->dwSignature == VS_FFI_SIGNATURE)
                    {
                        const auto major = HIWORD(versionInfo->dwFileVersionMS);
                        const auto minor = LOWORD(versionInfo->dwFileVersionMS);
                        const auto patch = HIWORD(versionInfo->dwFileVersionLS);
                        // revision

                        return VersionToRevision(major, minor, patch);
                    }
                }
            }
        }

        return std::nullopt;
    }

    Revision DetectRevision()
    {
        if (const auto forcedRevisionString = wil::TryGetEnvironmentVariableW(kForceRevisionEnvironmentVariable);
            forcedRevisionString != nullptr)
        {
            if (const auto parsedRevision = VersionStringToRevision(forcedRevisionString.get());
                parsedRevision.has_value())
            {
                return parsedRevision.value();
            }
        }

        if (const auto executableVersion = GetUnityRevisionFromExecutable();
            executableVersion.has_value())
        {
            return executableVersion.value();
        }

        FAIL_FAST_WIN32(ERROR_NOT_FOUND);
    }

    std::optional<Variant> GetUnityVariantFromExecutable()
    {
        const auto module = GetUnityModule();
        const auto unityModuleFilepath = wil::GetModuleFileNameW(module);
        const auto unityModuleName = std::filesystem::path(unityModuleFilepath.get()).filename();
        return ExecutableNameToVariant(unityModuleName.string());
    }

    Variant DetectVariant()
    {
        if (const auto forcedVariantString = wil::TryGetEnvironmentVariableW(kForceVariantEnvironmentVariable);
            forcedVariantString != nullptr)
        {
            if (const auto parsedVariant = VariantStringToVariant(forcedVariantString.get());
                parsedVariant.has_value())
            {
                return parsedVariant.value();
            }
        }

        if (const auto executableVariant = GetUnityVariantFromExecutable();
            executableVariant.has_value())
        {
            return executableVariant.value();
        }

        FAIL_FAST_WIN32(ERROR_NOT_FOUND);
    }

    decltype(&SetWindowLongPtrA) pSetWindowLongPtrA;

    LONG_PTR WINAPI DetourSetWindowLongPtrA(HWND, int, LONG_PTR)
    {
        const auto revision = DetectRevision();
        const auto variant = DetectVariant();

        RunDumper<WindowsDumper>(revision, variant);
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
