#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <map>
#include <string>
#include <cstdint>
#include <memory.h>
#include <filesystem>

#include <hierarchicalTriangularMesh.h>

bool startsWith(std::string s, std::string & prefix)
{
    return s.rfind("-"+prefix, 0) == 0;
}

template <class T>
struct Argument
{
    std::string name;
    T value;
};

template <class T>
void getArgument
(
    Argument<T> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
);

template <>
void getArgument<uint8_t>
(
    Argument<uint8_t> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c < count-1  && startsWith(commandLine[c], arg.name))
    {
        arg.value = std::stoi(commandLine[c+1]);
    }
}

template <>
void getArgument<std::filesystem::path>
(
    Argument<std::filesystem::path> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c < count-1  && startsWith(commandLine[c], arg.name))
    {
        arg.value = std::filesystem::path(commandLine[c+1]);
    }
}

template <>
void getArgument<BASE_MESH>
(
    Argument<BASE_MESH> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c < count-1  && startsWith(commandLine[c], arg.name))
    {
        arg.value = BASE_MESH(std::min(uint8_t(std::stoi(commandLine[c+1])), uint8_t(uint8_t(BASE_MESH::ANY))));
    }
}

template <>
void getArgument<bool>
(
    Argument<bool> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (startsWith(commandLine[c], arg.name))
    {
        arg.value = true;
    }
}

struct CommandLine
{

    CommandLine(uint8_t count, char ** commandLine)
    {
        if (count == 1) { return; }
        for (uint8_t c = 1; c < count; c++)
        {
            getArgument<uint8_t>(levelOfDetail, commandLine, c, count);
            getArgument<uint8_t>(msaa, commandLine, c, count);
            getArgument<bool>(meshes, commandLine, c, count);
            getArgument<BASE_MESH>(mesh, commandLine, c, count);
            getArgument<std::filesystem::path>(structure, commandLine, c, count);
        }
    }

    Argument<uint8_t> levelOfDetail = {"levelOfDetail", 0};
    Argument<uint8_t> msaa = {"msaa", 0};
    Argument<BASE_MESH> mesh = {"mesh", BASE_MESH::ANY};
    Argument<bool> meshes = {"meshes", false};
    Argument<std::filesystem::path> structure = {"atoms", {}};

};

#endif /* COMMANDLINE_H */
