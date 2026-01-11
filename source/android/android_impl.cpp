#include "dumper.hpp"
#include <bits/elf_common.h>
#include <filesystem>
#include <jni.h>
#include <android/log.h>
#include <link.h>
#include <linux/elf.h>
#include <span>
#include <thread>

#include "executable.hpp"
#include "xhook.h"

template<Revision R, Variant V>
class AndroidDumper
{
public:
    std::span<ExecutableSection> GetExecutableSections()
    {
        if (CachedSections.empty())
        {
            struct ElfInfo {
                std::span<const Elf64_Phdr> Sections;
                uintptr_t BaseAddress;
            };
            ElfInfo libraryInfo{};

            const auto findResult = dl_iterate_phdr([](dl_phdr_info* info, size_t size, void* context) -> int {
                if (std::string_view(info->dlpi_name).ends_with("/libunity.so")) {
                    const auto result = (ElfInfo*)context;
                    result->Sections = std::span(info->dlpi_phdr, info->dlpi_phnum);
                    result->BaseAddress = info->dlpi_addr;
                    return true;
                }

                return false;
            }, &libraryInfo);

            if (!findResult) {
                __android_log_print(ANDROID_LOG_DEBUG, "TypeTreeRipper", "Failed to find libunity.so :(");
                return {};
            }

            for (const auto& phdr : libraryInfo.Sections) {
                const auto base = phdr.p_vaddr + libraryInfo.BaseAddress;
                const auto size = phdr.p_memsz;

                auto protection = 0;
                if (phdr.p_flags & PF_R)
                    protection |= ExecutableSection::kSectionProtectionRead;

                if (phdr.p_flags & PF_W)
                    protection |= ExecutableSection::kSectionProtectionWrite;

                if (phdr.p_flags & PF_X)
                    protection |= ExecutableSection::kSectionProtectionExecute;

                CachedSections.emplace_back(std::span((char*)base, size), protection);
            }
        }

        return CachedSections;
    }

    static std::ofstream CreateOutputFile(char const *filename)
    {
        // On Android, we have to output our files into /data/data/<app package name>/files so that they are retrievable later.
        const auto packageName = []
        {
            std::ifstream cmdline("/proc/self/cmdline");
            std::string package;
            std::getline(cmdline, package, '\0');
            return package;
        }();

        const auto outputDirectory = std::filesystem::path("/data/data") / packageName / "files";
        const auto outputPath = outputDirectory / filename;
        return std::ofstream(outputPath);
    }

    static void DebugLog(char const *message)
    {
        __android_log_print(ANDROID_LOG_DEBUG, "TypeTreeRipper", "%s", message);
    }
private:
    std::vector<ExecutableSection> CachedSections;
};

namespace
{
    decltype(&__android_log_print) original_android_log_print;
    int hooked_android_log_print(int prio, const char *tag, const char *fmt, ...)
    {
        const auto res = original_android_log_print(prio, tag, fmt);
        if (std::string_view(fmt).starts_with("Product Name:"))
        {
            __android_log_print(prio, tag, "Detected Unity engine initialization, starting dumper...");
            RunDumper<AndroidDumper>(Revision::V6000_0, Variant::Runtime);
            std::abort();
        }

        return res;
    }

    void ThreadMain()
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        xhook_enable_debug(1);
        xhook_register(".*/libunity\\.so$", "__android_log_print", (void *)hooked_android_log_print, (void **)&original_android_log_print);
        xhook_refresh(0);
    }
}

extern "C" jint JNIEXPORT JNI_OnLoad(JavaVM* vm, void* reserved)
{
    static constexpr auto kKittyInjectorMagic = 1337;
    if (reinterpret_cast<uintptr_t>(reserved) == kKittyInjectorMagic)
    {
        setenv("UNITY_GIVE_CHANCE_TO_ATTACH_DEBUGGER", "1", true);
        std::thread(ThreadMain).detach();
    }

    return JNI_VERSION_1_6;
}
