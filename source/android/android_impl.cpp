#include "dumper.hpp"
#include <filesystem>

template<Revision R, Variant V>
class AndroidDumper
{
public:
    static std::span<ExecutableSection> GetExecutableSections()
    {
        throw std::logic_error("Not implemented");
    }

    static std::ofstream CreateOutputFile(char const *filename)
    {
        // On Android, we have to output our files into /data/local/tmp so that they are retrievable later.
        const auto outputDirectory = std::filesystem::path("/data/local/tmp/typetreeripper");

        // Create our output (sub-)directory if it doesn't exist.
        std::filesystem::create_directories(outputDirectory);

        return std::ofstream(outputDirectory / filename);
    }
};

int main()
{
    return 1;
}
