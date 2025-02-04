#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <map>
#include <string>
#include <cstdint>
#include <memory.h>
#include <filesystem>
#include <iostream>
#include <cstdlib>

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
    std::string description;
    T defaultValue;
    T value;
    bool required;
    uint8_t position;

    /**
     * @brief Construct a new Argument
     *
     * @tparam T the Argument's value type.
     * @param name the arguments name (without "-" prefix).
     * @param description the help description.
     * @param defaultValue the default value.
     * @param required if the Argument must be passed.
     * @param position if the Argument must be in a set position.
     */
    Argument
    (
        std::string name,
        std::string description,
        T defaultValue,
        bool required = false,
        uint8_t position = 0
    )
    : name(name),
      description(description),
      defaultValue(defaultValue),
      value(defaultValue),
      required(required),
      position(position)
    {}

    /**
     * @brief Return a formatted help message.
     *
     * @return std::string the help message including name, description, default value and requirement.
     */
    std::string help() const
    {
        std::stringstream h;
        if (position > 0)
        {
            h << "Positional argument " << int(position);
        }
        else
        {
            h << " -" << name;
        }
        h  << "\n  " << description;
        if (required)
        {
            h << "\n   Default: none."
              << "\n   Required: "
              << (required ? "true" : "false")
              << ".";
        }
        else
        {
            h << "\n   Default: "
              << defaultValue
              << "\n   Required: "
              << (required ? "true" : "false")
              << ".";
        }
        return h.str();
    }
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
 * @return true the argument was read."
 * @return false the argument was not read."
 */
template <class T>
bool getArgument
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
 * @return true the argument was read."
 * @return false the argument was not read."
 */
template <>
bool getArgument<uint8_t>
(
    Argument<uint8_t> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c == arg.position)
    {
        arg.value = std::stoi(commandLine[c]);
        return true;
    }
    if (c < count-1  && startsWith(commandLine[c], arg.name))
    {
        arg.value = std::stoi(commandLine[c+1]);
        return true;
    }
    return false;
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
 * @return true the argument was read."
 * @return false the argument was not read."
 */
template <>
bool getArgument<float>
(
    Argument<float> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c == arg.position)
    {
        arg.value = std::stof(commandLine[c]);
        return true;
    }
    if (c < count-1  && startsWith(commandLine[c], arg.name))
    {
        arg.value = std::stof(commandLine[c+1]);
        return true;
    }
    return false;
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
 * @remark If commandLine[c] does not exist as a path a std::runtime_error is thrown.
 * @return true the argument was read."
 * @return false the argument was not read."
 */
template <>
bool getArgument<std::filesystem::path>
(
    Argument<std::filesystem::path> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c == arg.position)
    {
        arg.value = std::filesystem::path(commandLine[c]);
        if (!std::filesystem::exists(arg.value))
        {
            throw std::runtime_error(std::string("Path: ") + commandLine[c] + " does not exist.");
        }
        return true;
    }
    if (c < count-1  && startsWith(commandLine[c], arg.name))
    {
        arg.value = std::filesystem::path(commandLine[c+1]);
        if (!std::filesystem::exists(arg.value))
        {
            throw std::runtime_error(std::string("Path: ") + commandLine[c] + " does not exist.");
        }
        return true;
    }
    return false;
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
 * @return true the argument was read."
 * @return false the argument was not read."
 */
template <>
bool getArgument<BASE_MESH>
(
    Argument<BASE_MESH> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c == arg.position)
    {
        arg.value = BASE_MESH(std::min(uint8_t(std::stoi(commandLine[c])), uint8_t(uint8_t(BASE_MESH::ANY))));
        return true;
    }
    if (c < count-1 && startsWith(commandLine[c], arg.name))
    {
        arg.value = BASE_MESH(std::min(uint8_t(std::stoi(commandLine[c+1])), uint8_t(uint8_t(BASE_MESH::ANY))));
        return true;
    }
    return false;
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
 * @return true the argument was read."
 * @return false the argument was not read."
 * @remark Acts as a switch, i.e. no argument is required.
 */
template <>
bool getArgument<bool>
(
    Argument<bool> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c == arg.position || startsWith(commandLine[c], arg.name))
    {
        arg.value = true;
        return true;
    }
    return false;
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
        if (count == 1) { help(); return; }
        for (uint8_t c = 1; c < count; c++)
        {
            checkHelp(commandLine[c]);
        }
        if (!getArgument<std::filesystem::path>(structure, commandLine, structure.position, count))
        {
            std::cout << "A structure path must be given as argument 1\n";
            std::exit(EXIT_FAILURE);
        }

        for (uint8_t c = 1; c < count; c++)
        {
            getArgument<uint8_t>(levelOfDetail, commandLine, c, count);
            getArgument<uint8_t>(msaa, commandLine, c, count);
            getArgument<bool>(meshes, commandLine, c, count);
            getArgument<BASE_MESH>(mesh, commandLine, c, count);
            getArgument<float>(bondCutoff, commandLine, c, count);
            getArgument<float>(bondSize, commandLine, c, count);
            getArgument<bool>(hideAtoms, commandLine, c, count);
            getArgument<float>(deemphasisAlpha, commandLine, c, count);
        }
    }

    Argument<uint8_t> levelOfDetail = {"levelOfDetail", "Level of detail for procedural meshes.", 0, false};
    Argument<uint8_t> msaa = {"msaa", "MSAA level [0-32].", 0, false};
    Argument<BASE_MESH> mesh = {"mesh", "The procedural mesh type.", BASE_MESH::ANY, false};
    Argument<bool> meshes = {"meshes", "Whether to use meshes for atoms.", false, false};
    Argument<std::filesystem::path> structure = {"atoms", "The structure path.", {}, true, 1};
    Argument<float> bondCutoff = {"bondCutOff","Angstrom cutoff to create a bond.", 0.0f, false};
    Argument<float> bondSize = {"bondSize", "The size of bonds.", 1.0f, false};
    Argument<bool> hideAtoms = {"hideAtoms", "Whether to hide atoms (toggle-able at runtime).", false, false};
    Argument<float> deemphasisAlpha = {"deemphasisAlpha", "Alpha colour channel for deemphasised atoms.", 0.25f, false};

    /**
     * @brief Determine if help should be printed.
     *
     * @param arg the argument to check.
     * @remark Calls std::exit(EXIT_SUCCESS);
     */
    void checkHelp(std::string arg)
    {
        if (arg == "-h" || arg == "-v" || arg == "-help" || arg == "-version")
        {
            help();
        }
    }

    /**
     * @brief The text banner.
     *
     */
    const char * banner = R"( ________  ________ ________  ________  ___      ___
|\   ____\|\  _____\\   __  \|\   __  \|\  \    /  /|
\ \  \___|\ \  \__/\ \  \|\  \ \  \|\  \ \  \  /  / /
 \ \_____  \ \   __\\ \  \\\  \ \   __  \ \  \/  / /
  \|____|\  \ \  \_| \ \  \\\  \ \  \ \  \ \    / /
    ____\_\  \ \__\   \ \_______\ \__\ \__\ \__/ /
   |\_________\|__|    \|_______|\|__|\|__|\|__|/
   \|_________| SimpleFastOpenAtomicVisualiser)";

    /**
     * @brief Print the help message and std::exit(EXIT_SUCCESS);.
     *
     */
    void help() const
    {
        std::stringstream h;
        h << banner
          << "\n\n"
          << "Repository: github.com/JerboaBurrow/SimpleFastOpenAtomicVisualiser\n"
          << "License: MIT, Jerboa 2025.\n"
          << VERSION
          << "\nUsage:\n"
          << structure.help()
          << "\n"
          << msaa.help()
          << "\n"
          << mesh.help()
          << "\n"
          << meshes.help()
          << "\n"
          << levelOfDetail.help()
          << "\n"
          << bondCutoff.help()
          << "\n"
          << bondSize.help()
          << "\n"
          << hideAtoms.help()
          << "\n"
          << deemphasisAlpha.help()
          << "\n";
        std::cout << h.str();
        std::exit(EXIT_SUCCESS);
    }

};

#endif /* COMMANDLINE_H */
