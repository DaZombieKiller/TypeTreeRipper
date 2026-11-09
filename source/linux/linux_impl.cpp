#include <cstdarg>
#include <cstring>
#include <filesystem>
#include <span>
#include <string_view>
#include <fstream>
#include <vector>

#include <link.h>
#include <dlfcn.h>
#include <iostream>
#include <signal.h>

#include "common.hpp"
#include "executable.hpp"
#include "dumper.hpp"

template<Revision R, Variant V>
class LinuxDumper
{
public:
    std::span<ExecutableSection> GetExecutableSections()
    {
        if (CachedSections.empty())
        {
            struct ElfInfo {
                std::span<const ElfW(Phdr)> Sections;
                uintptr_t BaseAddress{};
            };
            ElfInfo libraryInfo{};

            const auto findResult = dl_iterate_phdr([](dl_phdr_info* info, size_t size, void* context) -> int {
                if (info->dlpi_name[0] == 0 ||
                    std::string_view(info->dlpi_name).ends_with("/UnityPlayer.so") || std::string_view(info->dlpi_name).ends_with("/TuanjiePlayer.so")) {
                    const auto result = static_cast<ElfInfo*>(context);
                    result->Sections = std::span(info->dlpi_phdr, info->dlpi_phnum);
                    result->BaseAddress = info->dlpi_addr;
                    return true;
                }

                return false;
            }, &libraryInfo);

            if (!findResult) {
                DebugLog("Failed to find unity :(");
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
        // Create the output file in the current directory.
        // NOTE: Should this be made configurable? i.e. through an environment variable?
        const auto path = std::filesystem::current_path() / filename;
        dprintf(2, "[TypeTreeRipper] writing '%s'", path.c_str());
        return std::ofstream(path, std::ios::out | std::ios::binary);
    }

    static void DebugLog(char const *message)
    {
        dprintf(2, "[TypeTreeRipper] %s\n", message);
    }
private:
    std::vector<ExecutableSection> CachedSections;
};

typedef int (*orig_vfprintf_type)(FILE *stream, const char *format, va_list ap);

static Revision CachedRevision = Revision::Count;

int vfprintf(FILE *stream, const char *format, va_list ap) {
    orig_vfprintf_type orig_vfprintf = (orig_vfprintf_type)dlsym(RTLD_NEXT, "vfprintf");

    char tmp_buffer[0x100];
    va_list cap;
    va_copy(cap, ap);
    vsnprintf(tmp_buffer, sizeof(tmp_buffer), format, cap);
    va_end(cap);
    tmp_buffer[0xff] = 0;
    // "engine version:" -> Runtime: Version
    // "Editor version:" -> Editor: version
    // is there a better way??
    if (CachedRevision == Revision::Count && strstr(tmp_buffer, "engine version:")) {
        const auto str = std::string(tmp_buffer);
        if (const auto parsedRevision = VersionStringToRevision(str.substr(str.find(':') + 1)); parsedRevision.has_value())
        {
            CachedRevision = parsedRevision.value();
            RunDumper<LinuxDumper>(CachedRevision, Variant::Runtime);
            // this is likely too early.
            dprintf(2, "[TypeTreeRipper] parsed runtime revision");
            raise(SIGKILL);
        } else {
            dprintf(2, "[TypeTreeRipper] failed to parse revision");
        }
    } else if (CachedRevision == Revision::Count && strstr(tmp_buffer, "Editor version:")) {
        const auto str = std::string(tmp_buffer);
        if (const auto parsedRevision = VersionStringToRevision(str.substr(str.find(':') + 1)); parsedRevision.has_value())
        {
            CachedRevision = parsedRevision.value();
            dprintf(2, "[TypeTreeRipper] parsed editor revision");
        } else {
            dprintf(2, "[TypeTreeRipper] failed to parse revision");
        }
    }

    // note: this is late, it also re-initializes managers which can crash unity if it happens during mono reset
    // luckily this needle is printed after that
    if (CachedRevision != Revision::Count && strstr(tmp_buffer, "Finished resetting the current domain")) {
        RunDumper<LinuxDumper>(CachedRevision, Variant::Editor);
        raise(SIGKILL);
    }

    return orig_vfprintf(stream, format, ap);
}
