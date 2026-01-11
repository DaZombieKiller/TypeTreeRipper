#pragma once
#include "common.hpp"
#include "executable.hpp"

template<Revision R, Variant V, typename T>
concept IsPlatformImpl = requires(T impl, char const *filename)
{   
    { impl.GetExecutableSections() } -> std::convertible_to<std::span<ExecutableSection>>;
    { impl.CreateOutputFile(filename) } -> std::convertible_to<std::ofstream>;
    { impl.DebugLog(filename) } -> std::convertible_to<void>;
};
