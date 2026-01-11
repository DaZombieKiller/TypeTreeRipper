#include <cstdarg>
#include <filesystem>
#include <span>
#include <string_view>
#include <fstream>
#include <array>
#include <vector>
#include <sstream>

#include <android/api-level.h>
#include <bits/elf_common.h>
#include <jni.h>
#include <android/log.h>
#include <link.h>
#include <linux/elf.h>
#include <dlfcn.h>

#include "dobby.h"

#include "common.hpp"
#include "executable.hpp"
#include "dumper.hpp"

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
                    const auto result = static_cast<ElfInfo*>(context);
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

                CachedSections.emplace_back(std::span(reinterpret_cast<char*>(base), size), protection);
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
    Revision DetectedRevision = Revision::V0;
    Variant DetectedVariant = Variant::Runtime;

    decltype(&__android_log_print) original_android_log_print;
    decltype(&__android_log_vprint) original_android_log_vprint;
    __android_logger_function original_android_logger_function;

    void ProcessProductNameMessage(const std::string_view) 
    {
        __android_log_print(ANDROID_LOG_DEBUG, "Unity", "[TypeTreeRipper] Detected Unity engine initialization, starting dumper");
        RunDumper<AndroidDumper>(DetectedRevision, DetectedVariant);
        __android_log_print(ANDROID_LOG_DEBUG, "Unity", "[TypeTreeRipper] Dumper finished!");

        std::abort();
    }

    void ProcessBuiltFromMessage(const std::string_view msg) 
    {
        size_t previousOffset = 0;

        std::array<std::string_view, 6> components;
        for (int i = 0; i < components.size(); i++)
        {
            const auto start = msg.find('\'', previousOffset);
            if (start == std::string_view::npos)
                break;

            const auto end = msg.find('\'', start + 1);
            if (end == std::string_view::npos)
                break;

            components[i] = msg.substr(start + 1, end - start - 1);
            previousOffset = end + 1;
        }

        // Branch
        const auto version = components[1]; // Version
        const auto buildType = components[2]; // Build type
        // Scripting Backend
        // CPU
        // Stripping

        // 'Release' or 'Development'
        if (buildType.starts_with("Release"))
        {
            DetectedVariant = Variant::Runtime;
        }
        else if (buildType.starts_with("Development"))
        {
            DetectedVariant = Variant::RuntimeDev;
        }
        else
        {
            __android_log_print(ANDROID_LOG_DEBUG, "Unity", "[TypeTreeRipper] Invalid build type detected: %s", buildType.data());
        }

        const auto [parsedMajor, parsedMinor] = [version]
        {
            // spanstream doesn't exist in the android ndk :(
            const auto versionString = std::string(version);
            std::istringstream versionParser(versionString);

            std::string versionComponent;
            std::getline(versionParser, versionComponent, '.');
            const auto major = std::stoi(versionComponent);

            std::getline(versionParser, versionComponent, '.');
            const auto minor = std::stoi(versionComponent);

            return std::make_tuple(major, minor);
        }();

        // TODO: Replace this loop
        for (const auto &[major, minor, revision] : {
            std::make_tuple(3, 0, Revision::V3_0),
            std::make_tuple(3, 4, Revision::V3_4),
            std::make_tuple(3, 5, Revision::V3_5),
            std::make_tuple(4, 0, Revision::V4_0),
            std::make_tuple(5, 0, Revision::V5_0),
            std::make_tuple(5, 1, Revision::V5_1),
            std::make_tuple(5, 2, Revision::V5_2),
            std::make_tuple(5, 3, Revision::V5_3),
            std::make_tuple(5, 4, Revision::V5_4),
            std::make_tuple(5, 5, Revision::V5_5),
            std::make_tuple(2017, 1, Revision::V2017_1),
            std::make_tuple(2017, 3, Revision::V2017_3),
            std::make_tuple(2018, 2, Revision::V2018_2),
            std::make_tuple(2018, 3, Revision::V2018_3),
            std::make_tuple(2019, 1, Revision::V2019_1),
            std::make_tuple(2019, 2, Revision::V2019_2),
            std::make_tuple(2019, 3, Revision::V2019_3),
            std::make_tuple(2019, 4, Revision::V2019_4),
            std::make_tuple(2020, 1, Revision::V2020_1),
            std::make_tuple(2021, 1, Revision::V2021_1),
            std::make_tuple(2022, 1, Revision::V2022_1),
            std::make_tuple(2022, 2, Revision::V2022_2),
            std::make_tuple(2022, 3, Revision::V2022_3),
            std::make_tuple(2023, 1, Revision::V2023_1),
            std::make_tuple(6000, 0, Revision::V6000_0),
        })
        {
            if (parsedMajor > major
                || (parsedMajor == major && parsedMinor >= minor))
            {
                DetectedRevision = revision;
            }
        }

        __android_log_print(ANDROID_LOG_DEBUG, "Unity", "[TypeTreeRipper] Parsed revision %d and variant %d from info string",
            DetectedRevision, DetectedVariant);
    }

    void ProcessLogMessage(const std::string_view msg) 
    {
        if (msg.starts_with("Product Name:")
            // Older Unity versions unfortunately don't have the above log line, so we use
            // one from the IL2CPP init instead.
            || msg.starts_with("Java VM not initialized"))
        {
            ProcessProductNameMessage(msg);
        }

        if (msg.starts_with("Built from"))
        {
            ProcessBuiltFromMessage(msg);
        }
    }

    int hooked_android_log_print(int prio, const char *tag, const char *fmt, ...)
    {
        ProcessLogMessage(std::string_view(fmt));

        va_list va;
        va_start(va, fmt);
        const auto result = original_android_log_vprint(prio, tag, fmt, va);
        va_end(va);
        return result;
    }

    int hooked_android_log_vprint(int prio, const char *tag, const char *fmt, va_list ap)
    {
        // Unity likes to log pre-formatted strings as fmt: %s, ap: <actual>
        // The string we are looking for is also one of those.
        auto msg = std::string_view(fmt);
        if (msg == "%s")
        {
            va_list copy;
            va_copy(copy, ap);
            msg = va_arg(ap, char const *);
            va_end(ap);
            ap = copy;
        }

        ProcessLogMessage(std::string_view(msg));

        const auto result = original_android_log_vprint(prio, tag, fmt, ap);
        va_end(ap);
        return result;
    }
}

extern "C" void StartDumper()
{
    if (android_get_device_api_level() >= 30) 
    {
        const auto liblog = dlopen("liblog.so", 0);
        const auto p__android_log_set_logger = (decltype(&__android_log_set_logger))dlsym(liblog, "__android_log_set_logger");
        original_android_logger_function = (decltype(&__android_log_logd_logger))dlsym(liblog, "__android_log_logd_logger");
        p__android_log_set_logger([](const __android_log_message* msg)
        {
            ProcessLogMessage(msg->message);
            original_android_logger_function(msg);
        });
    }
    else 
    {
        // fallback for API level < 30

        const auto androidLogPrint = DobbySymbolResolver("liblog.so", "__android_log_print");
        const auto androidLogVPrint = DobbySymbolResolver("liblog.so", "__android_log_vprint");
        if (androidLogPrint == nullptr || androidLogVPrint == nullptr)
        {
            __android_log_print(ANDROID_LOG_DEBUG, "Unity", "[TypeTreeRipper] Failed to resolve android log functions :(");
            return;
        }

        DobbyHook(androidLogPrint, (void *)hooked_android_log_print, (void **)&original_android_log_print);
        DobbyHook(androidLogVPrint, (void *)hooked_android_log_vprint, (void **)&original_android_log_vprint);
    }
}

extern "C" jint JNIEXPORT JNI_OnLoad(JavaVM* vm, void* reserved)
{
    static constexpr auto kKittyInjectorMagic = 1337;
    if (reinterpret_cast<uintptr_t>(reserved) == kKittyInjectorMagic)
    {
        StartDumper();
    }

    return JNI_VERSION_1_6;
}
