#pragma once

#include <span>

struct ExecutableSection
{
    std::span<char> Data;

    enum SectionProtection : uint8_t
    {
        kSectionProtectionRead = 1 << 0,
        kSectionProtectionWrite = 1 << 1,
        kSectionProtectionExecute = 1 << 2,
    };
    uint8_t Protection;

    bool IsValidPointer(void const *ptr, const size_t size, const uint8_t expectedProtection = kSectionProtectionRead) const
    {
        if ((Protection & expectedProtection) != expectedProtection)
            return false;

        const auto offset = std::distance(static_cast<char const *>(Data.data()), static_cast<char const*>(ptr));
        if (offset < 0)
            return false;

        if (offset + size > Data.size())
            return false;

        return true;
    }
};
