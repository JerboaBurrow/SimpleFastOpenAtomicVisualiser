#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <map>
#include <string>
#include <cstdint>
#include <memory.h>
#include <filesystem>

#include <hierarchicalTriangularMesh.h>

/**
 * @brief true if a string starts with -[prefix].
 *
 * @param s string to check
 * @param prefix the prefix (without -)
 * @return true s is "-[prefix][...]"
 * @return false s is not "-[prefix][...]"
 */
bool startsWith(std::string s, std::string & prefix)
{
    return s.rfind("-"+prefix, 0) == 0;
}

/**
 * @brief A command line argument.
 *
 * @tparam T the value's type
 */
template <class T>
struct Argument
{
    std::string name;
    T value;
};

/**
 * @brief Extract the value of an argument.
 *
 * @tparam T the argument's type.
 * @param arg the Argument.
 * @param commandLine argv command line.
 * @param c the entry to check.
 * @param count the size of commandLine.
 * @remark If arg.name is not at commandLine[c] nothing happens.
 */
template <class T>
void getArgument
(
    Argument<T> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
);

/**
 * @brief Extract a uint8_t argument.
 *
 * @tparam uint8_t
 * @param arg the Argument.
 * @param commandLine argv command line.
 * @param c the entry to check.
 * @param count the size of commandLine.
 * @remark If arg.name is not at commandLine[c] nothing happens.
 */
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

/**
 * @brief Extract a float argument.
 *
 * @tparam float
 * @param arg the Argument.
 * @param commandLine argv command line.
 * @param c the entry to check.
 * @param count the size of commandLine.
 * @remark If arg.name is not at commandLine[c] nothing happens.
 */
template <>
void getArgument<float>
(
    Argument<float> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c < count-1  && startsWith(commandLine[c], arg.name))
    {
        arg.value = std::stof(commandLine[c+1]);
    }
}

/**
 * @brief Extract a std::filesystem::path argument.
 *
 * @tparam std::filesystem::path
 * @param arg the Argument.
 * @param commandLine argv command line.
 * @param c the entry to check.
 * @param count the size of commandLine.
 * @remark If arg.name is not at commandLine[c] nothing happens.
 */
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

/**
 * @brief Extract a BASE_MESH argument.
 *
 * @tparam BASE_MESH
 * @param arg the Argument.
 * @param commandLine argv command line.
 * @param c the entry to check.
 * @param count the size of commandLine.
 * @remark If arg.name is not at commandLine[c] nothing happens.
 */
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

/**
 * @brief Extract a bool argument.
 *
 * @tparam bool
 * @param arg the Argument.
 * @param commandLine argv command line.
 * @param c the entry to check.
 * @param count the size of commandLine.
 * @remark If arg.name is not at commandLine[c] nothing happens.
 * @remark Acts as a switch, i.e. no argument is required.
 */
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

/**
 * @brief Extract command line arguments.
 * @remark Values of arguments are check at construction.
 */
struct CommandLine
{

    /**
     * @brief Extract values from argv.
     *
     * @param count the size of commandLine.
     * @param commandLine the value of argv.
     */
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
            getArgument<float>(bondCutoff, commandLine, c, count);
        }
    }

    Argument<uint8_t> levelOfDetail = {"levelOfDetail", 0};
    Argument<uint8_t> msaa = {"msaa", 0};
    Argument<BASE_MESH> mesh = {"mesh", BASE_MESH::ANY};
    Argument<bool> meshes = {"meshes", false};
    Argument<std::filesystem::path> structure = {"atoms", {}};
    Argument<float> bondCutoff = {"bondCutOff", 0.0f};

};

#endif /* COMMANDLINE_H */
