#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <map>
#include <string>
#include <cstdint>
#include <memory>
#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <sstream>

#include <hierarchicalTriangularMesh.h>
#include <constants.h>

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
};

/**
 * @brief Return a formatted help message for a Argument.
 *
 * @tparam T the argument's type.
 * @param arg the Argument to return help on.
 * @return std::string the help message including name, description, default value and requirement.
 */
template <class T>
std::string argumentHelp(const Argument<T> & arg)
{
    std::stringstream h;
    if (arg.position > 0)
    {
        h << "Positional argument " << int(arg.position);
    }
    else
    {
        h << " -" << arg.name;
    }
    h  << "\n  " << arg.description;
    if (arg.required)
    {
        h << "\n   Default: none."
            << "\n   Required: "
            << (arg.required ? "true" : "false")
            << ".";
    }
    else
    {
        h << "\n   Default: "
            << arg.defaultValue
            << ".";
    }
    return h.str();
}

/**
 * @brief Return a formatted help message for a Argument.
 *
 * @param arg the Argument to return help on.
 * @return std::string the help message including name, description, default value and requirement.
 */
template <>
std::string argumentHelp<uint8_t>(const Argument<uint8_t> & arg)
{
    std::stringstream h;
    if (arg.position > 0)
    {
        h << "Positional argument " << int(arg.position);
    }
    else
    {
        h << " -" << arg.name;
    }
    h  << "\n  " << arg.description;
    if (arg.required)
    {
        h << "\n   Default: none."
            << "\n   Required: "
            << (arg.required ? "true" : "false")
            << ".";
    }
    else
    {
        h << "\n   Default: "
            << int(arg.defaultValue)
            << ".";
    }
    return h.str();
}

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
 * @brief Extract a uint64_t argument.
 *
 * @tparam uint64_t
 * @param arg the Argument.
 * @param commandLine argv command line.
 * @param c the entry to check.
 * @param count the size of commandLine.
 * @remark If arg.name is not at commandLine[c] nothing happens.
 * @return true the argument was read."
 * @return false the argument was not read."
 */
template <>
bool getArgument<uint64_t>
(
    Argument<uint64_t> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c == arg.position)
    {
        arg.value = std::stoull(commandLine[c]);
        return true;
    }
    if (c < count-1  && startsWith(commandLine[c], arg.name))
    {
        arg.value = std::stoull(commandLine[c+1]);
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
 * @brief Extract a std::string argument.
 *
 * @tparam std::string
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
bool getArgument<std::string>
(
    Argument<std::string> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    if (c == arg.position)
    {
        arg.value = std::string(commandLine[c]);
        return true;
    }
    if (c < count-1  && startsWith(commandLine[c], arg.name))
    {
        arg.value = std::string(commandLine[c+1]);
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

template <uint8_t L>
using vec = glm::vec<L, float, glm::qualifier::highp>;

/**
 * @brief Extract a float vector of size 2 argument.
 *
 * @tparam glm::vec<L, float, glm::qualifier::highp>
 * @param arg the Argument.
 * @param commandLine argv command line.
 * @param c the entry to check.
 * @param count the size of commandLine.
 * @remark If arg.name is not at commandLine[c] nothing happens.
 * @return true the argument was read."
 * @return false the argument was not read."
 */
template <>
bool getArgument<vec<2>>
(
    Argument<vec<2>> & arg,
    char ** commandLine,
    const uint8_t c,
    const uint8_t count
)
{
    std::stringstream data;
    if (c == arg.position)
    {
        data << std::string(commandLine[c]);
    }
    if (c < count-1  && startsWith(commandLine[c], arg.name))
    {
        data << std::string(commandLine[c+1]);
    }
    else
    {
        return false;
    }
    data >> arg.value.x;
    if (data.eof()) { throw std::runtime_error(arg.name+" requires 2 values like \"512 512\""); }
    data >> arg.value.y;
    return true;
}

/**
 * @brief Extract command line arguments.
 * @remark Values of arguments are check at construction.
 */
struct CommandLine
{

    CommandLine() = default;

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
            checkSpecial(commandLine[c]);
        }
        welcome();
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
            getArgument<bool>(showAxes, commandLine, c, count);
            getArgument<bool>(showCell, commandLine, c, count);
            getArgument<float>(deemphasisAlpha, commandLine, c, count);
            getArgument<std::filesystem::path>(colourmap, commandLine, c, count);
            getArgument<std::filesystem::path>(atomColours, commandLine, c, count);
            getArgument<float>(atomSize, commandLine, c, count);
            getArgument<vec<2>>(resolution, commandLine, c, count);
            getArgument<bool>(hideInfoText, commandLine, c, count);
            getArgument<bool>(play, commandLine, c, count);
            getArgument<uint64_t>(bondFocus, commandLine, c, count);
            getArgument<uint64_t>(focus, commandLine, c, count);
            getArgument<uint8_t>(speed, commandLine, c, count);
            getArgument<bool>(darkTheme, commandLine, c, count);
            getArgument<std::filesystem::path>(script, commandLine, c, count);
            getArgument<bool>(noTransparencySorting, commandLine, c, count);
            getArgument<bool>(sizeByMass, commandLine, c, count);
            #ifdef WITH_FFMPEG
            getArgument<uint8_t>(cq, commandLine, c, count);
            getArgument<uint8_t>(qp, commandLine, c, count);
            getArgument<uint8_t>(crf, commandLine, c, count);
            getArgument<uint64_t>(maxBFrames, commandLine, c, count);
            getArgument<uint64_t>(gopSize, commandLine, c, count);
            getArgument<std::string>(preset, commandLine, c, count);
            getArgument<std::string>(codec, commandLine, c, count);
            getArgument<std::string>(profile, commandLine, c, count);
            #endif
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
    Argument<bool> showAxes = {"showAxes", "Whether to show the coordinate axes (toggle-able at runtime).", false, false};
    Argument<bool> showCell = {"showCell", "Whether to show the simulation cell (toggle-able at runtime).", false, false};
    Argument<float> deemphasisAlpha = {"deemphasisAlpha", "Alpha colour channel for deemphasised atoms.", 0.25f, false};
    Argument<std::filesystem::path> colourmap = {"colourmap", "The colourmap path.", {}, false};
    Argument<std::filesystem::path> atomColours = {"atomColours", "Path for per-atom colour overrides.", {}, false};
    Argument<float> atomSize = {"atomSize", "Global atom size scaling factor.", 1.0f, false};
    Argument<vec<2>> resolution = {"resolution", "Window resolution in pixels.", {512, 512}, false};
    Argument<bool> hideInfoText = {"hideInfoText", "Hide information and statistics text (toggle-able at runtime).", false, false};
    Argument<bool> play = {"play", "Set to play trajectories at start up (toggle-able at runtime).", false, false};
    Argument<uint64_t> bondFocus = {"bondFocus", "Only draw bonds involving this atom index.", NULL_INDEX, false};
    Argument<uint64_t> focus = {"focus", "Centre on a particular atom.", NULL_INDEX, false};
    Argument<uint8_t> speed = {"speed", "Play speed between 1 and 60.", 60, false};
    Argument<bool> noCentering = {"noCentering", "Do not centre the atoms", false, false};
    Argument<bool> darkTheme = {"darkTheme", "Use dark theme", false, false};
    Argument<std::filesystem::path> script = {"script", "Path for a Lua script", {}, false};
    Argument<bool> noTransparencySorting = {"noTransparencySorting", "Disable transparency sorting for faster rendering.", false, false};
    Argument<bool> sizeByMass = {"sizeByMass", "Size elements by mass.", false, false};

    #ifdef WITH_FFMPEG
    Argument<std::string> codec = {"codec", "FFmpeg codec name (see ffmpeg -codecs).", "libx264", false};
    Argument<uint8_t> crf = {"crf", "Set the FFmpeg crf (0-51).", 0, false};
    Argument<uint8_t> qp = {"qp", "Set the FFmpeg qp (0-51).", 0, false};
    Argument<uint8_t> cq = {"cq", "Set the FFmpeg cp (0-51).", 0, false};
    Argument<uint64_t> maxBFrames = {"maxBFrames", "Set the FFmpeg maxBFrames.", 0, false};
    Argument<uint64_t> gopSize = {"gopSize", "Set the FFmpeg GOP size.", 1, false};
    Argument<std::string> preset = {"preset", "Set the FFmpeg preset.", "slow", false};
    Argument<std::string> profile = {"profile", "Set the FFmpeg profile.", "high", false};
    #endif

    /**
     * @brief Determine if help or licenses should be printed.
     *
     * @param arg the argument to check.
     * @remark Calls std::exit(EXIT_SUCCESS) after displaying information.
     */
    void checkSpecial(std::string arg)
    {
        if (arg == "-h" || arg == "-help")
        {
            help();
            std::exit(EXIT_SUCCESS);
        }
        if (arg == "-l" || arg == "-v" || arg == "-license" || arg == "-version")
        {
            welcome();
            licenses();
            std::exit(EXIT_SUCCESS);
        }
        if (arg == "-gpl")
        {
            gpl();
            std::exit(EXIT_SUCCESS);
        }
    }

    void welcome() const
    {
        std::stringstream w;
        w << banner
          << "\n"
          << VERSION
          << "\n -h and -help for help, -l/-v and -license/-version.\n\n";
          std::cout << w.str();
    }

    /**
     * @brief Print the help message and std::exit(EXIT_SUCCESS);.
     *
     */
    void help() const
    {
        std::stringstream h;
        h << banner
          << "\n"
          << VERSION
          << "\nUsage:\n"
          << argumentHelp(structure)
          << "\n"
          << argumentHelp(play)
          << "\n"
          << argumentHelp(colourmap)
          << "\n"
          << argumentHelp(atomColours)
          << "\n"
          << argumentHelp(mesh)
          << "\n"
          << argumentHelp(meshes)
          << "\n"
          << argumentHelp(levelOfDetail)
          << "\n"
          << argumentHelp(bondFocus)
          << "\n"
          << argumentHelp(focus)
          << "\n"
          << argumentHelp(bondCutoff)
          << "\n"
          << argumentHelp(bondSize)
          << "\n"
          << argumentHelp(atomSize)
          << "\n"
          << argumentHelp(sizeByMass)
          << "\n"
          << argumentHelp(hideAtoms)
          << "\n"
          << argumentHelp(showAxes)
          << "\n"
          << argumentHelp(showCell)
          << "\n"
          << argumentHelp(deemphasisAlpha)
          << "\n"
          << argumentHelp(noTransparencySorting)
          << "\n"
          << argumentHelp(hideInfoText)
          << "\n"
          << argumentHelp(resolution)
          << "\n"
          << argumentHelp(msaa)
          << "\n"
          << argumentHelp(darkTheme)
          << "\n"
          << argumentHelp(speed)
          << "\n"
          << argumentHelp(noCentering)
          << "\n"
          << argumentHelp(script)
          #ifdef WITH_FFMPEG
          << "\n"
          << argumentHelp(codec)
          << "\n"
          << argumentHelp(crf)
          << "\n"
          << argumentHelp(qp)
          << "\n"
          << argumentHelp(cq)
          << "\n"
          << argumentHelp(maxBFrames)
          << "\n"
          << argumentHelp(gopSize)
          << "\n"
          << argumentHelp(preset)
          << "\n"
          << argumentHelp(profile)
          #endif
          << "\n";
        std::cout << h.str();
    }

    void licenses()
    {
        std::cout << ossLicenses << "\n";
    }

    void gpl()
    {
        std::cout << GPL << "\n";
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
   \|_________| SimpleFastOpenAtomicVisualiser

Copyright (C) 2025 Jerboa
SimpleFastOpenAtomicVisualiser comes with ABSOLUTELY NO WARRANTY;
for details pass the argument `-gpl'.  This is free software,
and you are welcome to redistribute it under certain conditions;
pass the argument `-gpl' for details.

Repository: github.com/JerboaBurrow/SimpleFastOpenAtomicVisualiser)";

    /**
     * @brief The OSS licenses.
     *
     */
    const char * ossLicenses = R"(
SimpleFastOpenAtomicVisuliser would not be possible
without the following Open Source Software

FreeType
  The FreeType Project License
  Copyright 1996-2002, 2006 by David Turner, Robert Wilhelm, and Werner Lemberg.

GLEW
  Copyright (C) 2002-2007, Milan Ikits <milan ikits[]ieee org>
  Copyright (C) 2002-2007, Marcelo E. Magallon <mmagallo[]debian org>
  Copyright (C) 2002, Lev Povalahev.

Mesa 3-D graphics library
  MIT
  Copyright (C) 1999-2007  Brian Paul
  Copyright (c) 2007 The Khronos Group Inc.

Vulkan SDK
  Full list https://vulkan.lunarg.com/software/license/vulkan-1.4.304.1-linux-license-summary.txt
  Copyright 2016-2025 LunarG Inc.

GLFW
  zlib/libpng license
  Copyright © 2002-2006 Marcus Geelnard
  Copyright © 2006-2019 Camilla Löwy.

OpenDyslexic
  SIL Open Font License, Version 1.1
  Copyright (c) 2019-07-29, Abbie Gonzalez
  (https://abbiecod.es|support@abbiecod.es),
  with Reserved Font Name OpenDyslexic.
  Copyright (c) 12/2012 - 2019

Impostor sphere OpenGL code is based on previous work
  MIT
  Copyright (C) 2010-2012 by Jason L. McKesson.

STDUUID
  MIT
  Copyright (c) 2017 Marius Bancila.

GLM
  MIT, but no bunnies were harmed either.
  Copyright (c) 2005 - G-Truc Creation.
  GLM has been modified to add std::ostream utilities for glm::vec.

jGL, jLog, jThread
  MIT
  Copyright Jerboa 2023, 2024, 2024.

STBIMAGE
  Public domain
  by Sean Barrett (nothings).

Lua
  MIT
  Copyright © 1994-2023 Lua.org, PUC-Rio.

FFmpeg
  GPL v2
  FFmpeg is a trademark of Fabrice Bellard.

jo_mpeg
  Public domain.
  Jon Olick.

With thanks, Jerboa.
)";

    /**
     * @brief GPL v2 license text.
     *
     */
    const char * GPL = R"(GNU GENERAL PUBLIC LICENSE

Version 2, June 1991

Copyright (C) 1989, 1991 Free Software Foundation, Inc.
<https://fsf.org/>
Everyone is permitted to copy and distribute verbatim copies
of this license document, but changing it is not allowed.

Preamble

The licenses for most software are designed to take away your freedom to share and change it. By contrast, the GNU General Public License is intended to guarantee your freedom to share and change free software--to make sure the software is free for all its users. This General Public License applies to most of the Free Software Foundation's software and to any other program whose authors commit to using it. (Some other Free Software Foundation software is covered by the GNU Lesser General Public License instead.) You can apply it to your programs, too.

When we speak of free software, we are referring to freedom, not price. Our General Public Licenses are designed to make sure that you have the freedom to distribute copies of free software (and charge for this service if you wish), that you receive source code or can get it if you want it, that you can change the software or use pieces of it in new free programs; and that you know you can do these things.

To protect your rights, we need to make restrictions that forbid anyone to deny you these rights or to ask you to surrender the rights. These restrictions translate to certain responsibilities for you if you distribute copies of the software, or if you modify it.

For example, if you distribute copies of such a program, whether gratis or for a fee, you must give the recipients all the rights that you have. You must make sure that they, too, receive or can get the source code. And you must show them these terms so they know their rights.

We protect your rights with two steps: (1) copyright the software, and (2) offer you this license which gives you legal permission to copy, distribute and/or modify the software.

Also, for each author's protection and ours, we want to make certain that everyone understands that there is no warranty for this free software. If the software is modified by someone else and passed on, we want its recipients to know that what they have is not the original, so that any problems introduced by others will not reflect on the original authors' reputations.

Finally, any free program is threatened constantly by software patents. We wish to avoid the danger that redistributors of a free program will individually obtain patent licenses, in effect making the program proprietary. To prevent this, we have made it clear that any patent must be licensed for everyone's free use or not licensed at all.

The precise terms and conditions for copying, distribution and modification follow.
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. This License applies to any program or other work which contains a notice placed by the copyright holder saying it may be distributed under the terms of this General Public License. The "Program", below, refers to any such program or work, and a "work based on the Program" means either the Program or any derivative work under copyright law: that is to say, a work containing the Program or a portion of it, either verbatim or with modifications and/or translated into another language. (Hereinafter, translation is included without limitation in the term "modification".) Each licensee is addressed as "you".

Activities other than copying, distribution and modification are not covered by this License; they are outside its scope. The act of running the Program is not restricted, and the output from the Program is covered only if its contents constitute a work based on the Program (independent of having been made by running the Program). Whether that is true depends on what the Program does.

1. You may copy and distribute verbatim copies of the Program's source code as you receive it, in any medium, provided that you conspicuously and appropriately publish on each copy an appropriate copyright notice and disclaimer of warranty; keep intact all the notices that refer to this License and to the absence of any warranty; and give any other recipients of the Program a copy of this License along with the Program.

You may charge a fee for the physical act of transferring a copy, and you may at your option offer warranty protection in exchange for a fee.

2. You may modify your copy or copies of the Program or any portion of it, thus forming a work based on the Program, and copy and distribute such modifications or work under the terms of Section 1 above, provided that you also meet all of these conditions:

    a) You must cause the modified files to carry prominent notices stating that you changed the files and the date of any change. 
    b) You must cause any work that you distribute or publish, that in whole or in part contains or is derived from the Program or any part thereof, to be licensed as a whole at no charge to all third parties under the terms of this License. 
    c) If the modified program normally reads commands interactively when run, you must cause it, when started running for such interactive use in the most ordinary way, to print or display an announcement including an appropriate copyright notice and a notice that there is no warranty (or else, saying that you provide a warranty) and that users may redistribute the program under these conditions, and telling the user how to view a copy of this License. (Exception: if the Program itself is interactive but does not normally print such an announcement, your work based on the Program is not required to print an announcement.) 

These requirements apply to the modified work as a whole. If identifiable sections of that work are not derived from the Program, and can be reasonably considered independent and separate works in themselves, then this License, and its terms, do not apply to those sections when you distribute them as separate works. But when you distribute the same sections as part of a whole which is a work based on the Program, the distribution of the whole must be on the terms of this License, whose permissions for other licensees extend to the entire whole, and thus to each and every part regardless of who wrote it.

Thus, it is not the intent of this section to claim rights or contest your rights to work written entirely by you; rather, the intent is to exercise the right to control the distribution of derivative or collective works based on the Program.

In addition, mere aggregation of another work not based on the Program with the Program (or with a work based on the Program) on a volume of a storage or distribution medium does not bring the other work under the scope of this License.

3. You may copy and distribute the Program (or a work based on it, under Section 2) in object code or executable form under the terms of Sections 1 and 2 above provided that you also do one of the following:

    a) Accompany it with the complete corresponding machine-readable source code, which must be distributed under the terms of Sections 1 and 2 above on a medium customarily used for software interchange; or, 
    b) Accompany it with a written offer, valid for at least three years, to give any third party, for a charge no more than your cost of physically performing source distribution, a complete machine-readable copy of the corresponding source code, to be distributed under the terms of Sections 1 and 2 above on a medium customarily used for software interchange; or, 
    c) Accompany it with the information you received as to the offer to distribute corresponding source code. (This alternative is allowed only for noncommercial distribution and only if you received the program in object code or executable form with such an offer, in accord with Subsection b above.) 

The source code for a work means the preferred form of the work for making modifications to it. For an executable work, complete source code means all the source code for all modules it contains, plus any associated interface definition files, plus the scripts used to control compilation and installation of the executable. However, as a special exception, the source code distributed need not include anything that is normally distributed (in either source or binary form) with the major components (compiler, kernel, and so on) of the operating system on which the executable runs, unless that component itself accompanies the executable.

If distribution of executable or object code is made by offering access to copy from a designated place, then offering equivalent access to copy the source code from the same place counts as distribution of the source code, even though third parties are not compelled to copy the source along with the object code.

4. You may not copy, modify, sublicense, or distribute the Program except as expressly provided under this License. Any attempt otherwise to copy, modify, sublicense or distribute the Program is void, and will automatically terminate your rights under this License. However, parties who have received copies, or rights, from you under this License will not have their licenses terminated so long as such parties remain in full compliance.

5. You are not required to accept this License, since you have not signed it. However, nothing else grants you permission to modify or distribute the Program or its derivative works. These actions are prohibited by law if you do not accept this License. Therefore, by modifying or distributing the Program (or any work based on the Program), you indicate your acceptance of this License to do so, and all its terms and conditions for copying, distributing or modifying the Program or works based on it.

6. Each time you redistribute the Program (or any work based on the Program), the recipient automatically receives a license from the original licensor to copy, distribute or modify the Program subject to these terms and conditions. You may not impose any further restrictions on the recipients' exercise of the rights granted herein. You are not responsible for enforcing compliance by third parties to this License.

7. If, as a consequence of a court judgment or allegation of patent infringement or for any other reason (not limited to patent issues), conditions are imposed on you (whether by court order, agreement or otherwise) that contradict the conditions of this License, they do not excuse you from the conditions of this License. If you cannot distribute so as to satisfy simultaneously your obligations under this License and any other pertinent obligations, then as a consequence you may not distribute the Program at all. For example, if a patent license would not permit royalty-free redistribution of the Program by all those who receive copies directly or indirectly through you, then the only way you could satisfy both it and this License would be to refrain entirely from distribution of the Program.

If any portion of this section is held invalid or unenforceable under any particular circumstance, the balance of the section is intended to apply and the section as a whole is intended to apply in other circumstances.

It is not the purpose of this section to induce you to infringe any patents or other property right claims or to contest validity of any such claims; this section has the sole purpose of protecting the integrity of the free software distribution system, which is implemented by public license practices. Many people have made generous contributions to the wide range of software distributed through that system in reliance on consistent application of that system; it is up to the author/donor to decide if he or she is willing to distribute software through any other system and a licensee cannot impose that choice.

This section is intended to make thoroughly clear what is believed to be a consequence of the rest of this License.

8. If the distribution and/or use of the Program is restricted in certain countries either by patents or by copyrighted interfaces, the original copyright holder who places the Program under this License may add an explicit geographical distribution limitation excluding those countries, so that distribution is permitted only in or among countries not thus excluded. In such case, this License incorporates the limitation as if written in the body of this License.

9. The Free Software Foundation may publish revised and/or new versions of the General Public License from time to time. Such new versions will be similar in spirit to the present version, but may differ in detail to address new problems or concerns.

Each version is given a distinguishing version number. If the Program specifies a version number of this License which applies to it and "any later version", you have the option of following the terms and conditions either of that version or of any later version published by the Free Software Foundation. If the Program does not specify a version number of this License, you may choose any version ever published by the Free Software Foundation.

10. If you wish to incorporate parts of the Program into other free programs whose distribution conditions are different, write to the author to ask for permission. For software which is copyrighted by the Free Software Foundation, write to the Free Software Foundation; we sometimes make exceptions for this. Our decision will be guided by the two goals of preserving the free status of all derivatives of our free software and of promoting the sharing and reuse of software generally.

NO WARRANTY

11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
END OF TERMS AND CONDITIONS)";

};

#endif /* COMMANDLINE_H */
