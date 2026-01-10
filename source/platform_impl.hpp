#pragma once
#include "common.hpp"
#include "executable.hpp"

template<Revision R, Variant V, typename T>
concept IsPlatformImpl = requires(T impl)
{   
    { impl.GetExecutableSections() } -> std::convertible_to<std::span<ExecutableSection>>;
};
