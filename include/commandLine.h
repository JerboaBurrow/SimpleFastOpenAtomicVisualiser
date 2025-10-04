#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <map>
#include <string>
#include <cstdint>
#include <memory>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <sstream>

#include <hierarchicalTriangularMesh.h>
#include <constants.h>
#include <util.h>
#include <lua.h>
#include <LuaBool.h>
#include <LuaNumber.h>
#include <LuaString.h>
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
    std::string note;
    T defaultValue;
    T value;

    /**
     * @brief Construct a new Argument
     *
     * @tparam T the Argument's value type.
     * @param name the arguments name (without "-" prefix).
     * @param description the help description.
     * @param note the argument note.
     * @param defaultValue the default value.
     */
    Argument
    (
        std::string name,
        std::string description,
        std::string note,
        T defaultValue
    )
    : name(name),
      description(description),
      note(note),
      defaultValue(defaultValue),
      value(defaultValue)
    {}
};

/**
 * @brief Return a formatted help message for a Argument.
 *
 * @tparam T the argument's type.
 * @param arg the Argument to return help on.
 * @return std::string the help message including name, description, and default value.
 */
template <class T>
std::string argumentHelp(const Argument<T> & arg)
{
    std::stringstream h;
    h << " -"
      << arg.name << "\n  "
      << arg.description
      << "\n   Default: "
      << arg.defaultValue
      << ".";
      if (arg.note != "")
      {
        h << "\n   Note: "
        << arg.note;
      }
    return h.str();
}

/**
 * @brief Return a formatted help message for a uint8_t Argument.
 *
 * @param arg the Argument to return help on.
 * @return std::string the help message including name, description, and default value.
 */
template <>
std::string argumentHelp<uint8_t>(const Argument<uint8_t> & arg)
{
    std::stringstream h;
    h << " -"
      << arg.name << "\n  "
      << arg.description
      << "\n   Default: "
      << int(arg.defaultValue)
      << ".";
      if (arg.note != "")
      {
        h << "\n   Note: "
        << arg.note;
      }
    return h.str();
}

/**
 * @brief Return a formatted help message for a flag Argument.
 *
 * @param arg the Argument to return help on.
 * @return std::string the help message including name, description, and default value.
 */
template <>
std::string argumentHelp<bool>(const Argument<bool> & arg)
{
    std::stringstream h;
    h << " --"
      << arg.name << "\n  "
      << arg.description
      << "\n   Default: "
      << (arg.defaultValue ? "True" : "False")
      << ".";
      if (arg.note != "")
      {
        h << "\n   Note: "
        << arg.note;
      }
    return h.str();
}

/**
 * @brief Extract the value of an argument.
 *
 * @tparam T the argument's type.
 * @param arg the Argument.
 * @param value argv command line value to extract from.
 */
template <class T>
void getArgument (Argument<T> & arg, const char * value);

/**
 * @brief Extract the value of a uint8_t argument.
 *
 * @tparam T uint8_t
 * @param arg the Argument.
 * @param value argv command line value to extract from.
 */
template <>
void getArgument<uint8_t> (Argument<uint8_t> & arg, const char * value) { arg.value = std::stoi(value); }

/**
 * @brief Extract the value of a uint64_t argument.
 *
 * @tparam T uint64_t
 * @param arg the Argument.
 * @param value argv command line value to extract from.
 */
template <>
void getArgument<uint64_t> (Argument<uint64_t> & arg, const char * value) { arg.value = std::stoull(value); }

/**
 * @brief Extract the value of a float argument.
 *
 * @tparam T float
 * @param arg the Argument.
 * @param value argv command line value to extract from.
 */
template <>
void getArgument<float> (Argument<float> & arg, const char * value) { arg.value = std::stof(value); }

/**
 * @brief Extract the value of a std::filesystem::path argument.
 *
 * @tparam T std::filesystem::path
 * @param arg the Argument.
 * @param value argv command line value to extract from.
 */
template <>
void getArgument<std::filesystem::path> (Argument<std::filesystem::path> & arg, const char * value)
{
    auto path = std::filesystem::path(value);
    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error(std::string("Path: ") + value + " does not exist.");
    }
    arg.value = path;
}

/**
 * @brief Extract the value of a std::string argument.
 *
 * @tparam T std::string
 * @param arg the Argument.
 * @param value argv command line value to extract from.
 */
template <>
void getArgument<std::string> (Argument<std::string> & arg, const char * value) { arg.value = std::string(value); }

/**
 * @brief Extract the value of a BASE_MESH argument.
 *
 * @tparam T BASE_MESH
 * @param arg the Argument.
 * @param value argv command line value to extract from.
 */
template <>
void getArgument<BASE_MESH> (Argument<BASE_MESH> & arg, const char * value)
{
    arg.value = BASE_MESH(std::min(uint8_t(std::stoi(value)), uint8_t(uint8_t(BASE_MESH::ANY))));
}

template <uint8_t L>
using vec = glm::vec<L, float, glm::qualifier::highp>;

/**
 * @brief Extract the value of a float vector of size 2 argument.
 *
 * @tparam T glm::vec<L, float, glm::qualifier::highp>
 * @param arg the Argument.
 * @param value argv command line value to extract from.
 */
template <>
void getArgument<vec<2>> (Argument<vec<2>> & arg, const char * value)
{
    std::stringstream data;
    float x, y;
    data << std::string(value);
    data >> x;
    if (data.eof()) { throw std::runtime_error(std::string(value)+" not a valid vec<2>, format is \"512 512\""); }
    data >> y;
    arg.value = vec<2>(x, y);
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
        getArgument<std::filesystem::path>(structure, commandLine[1]);

        std::vector<std::string> flags;
        std::map<std::string, const char *> arguments;

        for (uint8_t c = 2; c < count; c++)
        {
            std::string arg = commandLine[c];
            if (isFlag(arg))
            {
                flags.push_back(stripPrefix(commandLine[c]));
                if (c+1 < count && isValue(commandLine[c+1]))
                {
                    std::cout << "Option " << arg << " looks like a flag, it should not have a value.\n";
                    std::exit(EXIT_FAILURE);
                }
            }
            else
            {
                if (c+1 < count && isValue(commandLine[c+1]))
                {
                    arguments[stripPrefix(commandLine[c])] = commandLine[c+1];
                    c += 1;
                }
                else
                {
                    std::cout << "Option " << arg << " is missing its value.\n";
                    std::exit(EXIT_FAILURE);
                }
            }
        }
        extractFlag(hideAtoms, flags);
        extractFlag(showAxes, flags);
        extractFlag(showCell, flags);
        extractFlag(hideInfoText, flags);
        extractFlag(play, flags);
        extractFlag(meshes, flags);
        extractFlag(darkTheme, flags);
        extractFlag(noTransparencySorting, flags);
        extractFlag(sizeByMass, flags);
        extractFlag(noCentering, flags);

        extractArgument(levelOfDetail, arguments);
        extractArgument(msaa, arguments);
        extractArgument(mesh, arguments);
        extractArgument(bondCutoff, arguments);
        extractArgument(bondSize, arguments);
        extractArgument(deemphasisAlpha, arguments);
        extractArgument(atomClipCorrection, arguments);
        extractArgument(bondClipCorrection, arguments);
        extractArgument(fieldOfView, arguments);
        extractArgument(colourmap, arguments);
        extractArgument(atomColours, arguments);
        extractArgument(atomSize, arguments);
        extractArgument(resolution, arguments);
        extractArgument(bondFocus, arguments);
        extractArgument(focus, arguments);
        extractArgument(speed, arguments);
        extractArgument(script, arguments);
        extractArgument(globalAtomAlpha, arguments);
        extractArgument(globalBondAlpha, arguments);
        extractArgument(videoName, arguments);
        extractArgument(cameraZoomSpeed, arguments);
        extractArgument(cameraPanSpeed, arguments);
        extractArgument(cameraInclineSpeed, arguments);
        extractArgument(cameraRotateSpeed, arguments);

        #ifdef WITH_FFMPEG
        extractArgument(cq, arguments);
        extractArgument(qp, arguments);
        extractArgument(crf, arguments);
        extractArgument(maxBFrames, arguments);
        extractArgument(gopSize, arguments);
        extractArgument(preset, arguments);
        extractArgument(codec, arguments);
        extractArgument(profile, arguments);
        #endif
    }

    Argument<std::filesystem::path> structure = {"atoms", "Structure path.", "", {}};
    Argument<std::filesystem::path> script = {"script", "Path to Lua script", "Called each frame.", {}};

    Argument<uint8_t> levelOfDetail = {"levelOfDetail", "Level of detail of meshes.", "", 0};
    Argument<uint8_t> msaa = {"msaa", "MSAA level [0-32].", "", 0};
    Argument<uint8_t> speed = {"speed", "Play speed between 1 and 60.", "", 60};
    Argument<BASE_MESH> mesh = {"mesh", "Procedural mesh type.", "", BASE_MESH::ANY};
    Argument<float> bondCutoff = {"bondCutOff","Cutoff to create a bond.", "", 0.0f};
    Argument<float> bondSize = {"bondSize", "Size of bonds.", "", 1.0f};
    Argument<float> atomSize = {"atomSize", "Global atom size scaling factor.", "", 1.0f};
    Argument<float> globalAtomAlpha = {"globalAtomAlpha", "Alpha colour multiplier for atoms", "", 1.0f};
    Argument<float> globalBondAlpha = {"globalBondAlpha", "Alpha colour multiplier for bonds", "", 1.0f};
    Argument<float> deemphasisAlpha = {"deemphasisAlpha", "Alpha colour for deemphasised atoms.", "", 0.25f};
    Argument<float> atomClipCorrection = {"atomClipCorrection", "Correction for atom impostors.", "Increase if atoms clipped.", 1.5f};
    Argument<float> bondClipCorrection = {"bondClipCorrection", "Correction for bond impostors.", "Increase if atoms clipped.", 5.0f};
    Argument<float> fieldOfView = {"fieldOfView", "Field of view in degrees.", "", 60.0f};
    Argument<std::filesystem::path> colourmap = {"colourmap", "The colourmap path.", "", {}};
    Argument<std::filesystem::path> atomColours = {"atomColours", "Path for per-atom colour overrides.", "", {}};
    Argument<std::string> videoName = {"videoName", "Name of saved video.", "", ""};
    Argument<vec<2>> resolution = {"resolution", "Window resolution in pixels.", "", {512, 512}};
    Argument<uint64_t> bondFocus = {"bondFocus", "Only draw bonds for this atom.", "", NULL_INDEX};
    Argument<uint64_t> focus = {"focus", "Centre on a particular atom.", "", NULL_INDEX};
    Argument<float> cameraZoomSpeed = {"cameraZoomSpeed", "Speed of the camera's zoom.", "", 1.0};
    Argument<float> cameraRotateSpeed = {"cameraRotateSpeed", "Speed of the camera's rotation", "", 1.0};
    Argument<float> cameraInclineSpeed = {"cameraInclineSpeed", "Speed of the camera's inclination.", "", 1.0};
    Argument<float> cameraPanSpeed = {"cameraPanSpeed", "Speed of the camera's pan.", "", 1.0};

    #ifdef WITH_FFMPEG
    Argument<std::string> codec = {"codec", "FFmpeg codec name.", "See ffmpeg -codecs", "libx264"};
    Argument<uint8_t> crf = {"crf", "Set the FFmpeg crf (0-51).", "Use with e.g. libx264", 18};
    Argument<uint8_t> qp = {"qp", "Set the FFmpeg qp (0-51).", "Use with e.g. h264_nvenc", 18};
    Argument<uint8_t> cq = {"cq", "Set the FFmpeg cp (0-51).", "", 0};
    Argument<uint64_t> maxBFrames = {"maxBFrames", "Set the FFmpeg maxBFrames.", "", 0};
    Argument<uint64_t> gopSize = {"gopSize", "Set the FFmpeg GOP size.", "", 1};
    Argument<std::string> preset = {"preset", "Set the FFmpeg preset.", "", "slow"};
    Argument<std::string> profile = {"profile", "Set the FFmpeg profile.", "", "main"};
    #endif

    Argument<bool> meshes = {"meshes", "Use meshes for atoms.", "Toggleable at runtime.", false};
    Argument<bool> hideAtoms = {"hideAtoms", "Hide atoms.", "Toggleable at runtime.", false};
    Argument<bool> showAxes = {"showAxes", "Show the coordinate axes.", "Toggleable at runtime.", false};
    Argument<bool> showCell = {"showCell", "Show the simulation cell.", "Toggleable at runtime.", false};
    Argument<bool> hideInfoText = {"hideInfoText", "Hide information and statistics text.", "Toggleable at runtime.", false};
    Argument<bool> play = {"play", "Play trajectory.", "Toggleable at runtime.", false};
    Argument<bool> noCentering = {"noCentering", "Do not centre the atoms", "Toggleable at runtime.", false};
    Argument<bool> darkTheme = {"darkTheme", "Use dark theme", "Toggleable at runtime.", false};
    Argument<bool> noTransparencySorting = {"noTransparencySorting", "Disable transparency sorting.", "Toggleable at runtime.", false};
    Argument<bool> sizeByMass = {"sizeByMass", "Size elements by mass.", "Toggleable at runtime.", false};

    /**
     * @brief Set an option from Lua.
     *
     * @remark Lua arguments are:
     * 1. option name
     * 2. option value
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_setOption(lua_State * lua)
    {
        int args = lua_gettop(lua);
        if (args != 2)
        {
            const std::string msg = "setOption expects a name and value as argument.\n";
            lua_pushlstring(lua, msg.c_str(), msg.length());
            return lua_error(lua);
        }
        LuaString s; s.read(lua, 1);

        bool * bval = boolFromName(s.characters);
        if (bval != nullptr)
        {
            if (lua_isboolean(lua, 2))
            {
                LuaBool b; b.read(lua, 2);
                *bval = b.bit;
                return 0;
            }
            else
            {
                const std::string msg = "setOption for " + s.characters + " expects boolean value.\n";
                lua_pushlstring(lua, msg.c_str(), msg.length());
                return lua_error(lua);
            }
        }

        float * fval = floatFromName(s.characters);
        if (fval != nullptr)
        {
            if (lua_isnumber(lua, 2))
            {
                LuaNumber n; n.read(lua, 2);
                *fval = n.n;
                return 0;
            }
            else
            {
                const std::string msg = "setOption for " + s.characters + " expects numeric value.\n";
                lua_pushlstring(lua, msg.c_str(), msg.length());
                return lua_error(lua);
            }
        }

        uint8_t * i8val = i8FromName(s.characters);
        if (i8val != nullptr)
        {
            if (lua_isnumber(lua, 2))
            {
                LuaNumber n; n.read(lua, 2);
                *i8val = n.n;
                return 0;
            }
            else
            {
                const std::string msg = "setOption for " + s.characters + " expects numeric value.\n";
                lua_pushlstring(lua, msg.c_str(), msg.length());
                return lua_error(lua);
            }
        }

        uint64_t * i64val = i64FromName(s.characters);
        if (i64val != nullptr)
        {
            if (lua_isnumber(lua, 2))
            {
                LuaNumber n; n.read(lua, 2);
                *i64val = n.n;
                return 0;
            }
            else
            {
                const std::string msg = "setOption for " + s.characters + " expects numeric value.\n";
                lua_pushlstring(lua, msg.c_str(), msg.length());
                return lua_error(lua);
            }
        }

        std::string * sval = stringFromName(s.characters);
        if (sval != nullptr)
        {
            if (lua_isstring(lua, 2))
            {
                LuaString v; v.read(lua, 2);
                *sval = v.characters;
                return 0;
            }
            else
            {
                const std::string msg = "setOption for " + s.characters + " expects string value.\n";
                lua_pushlstring(lua, msg.c_str(), msg.length());
                return lua_error(lua);
            }
        }
        const std::string msg = "setOption, no option " + s.characters + ".\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    /**
     * @brief Get an option value in Lua.
     *
     * @remark Lua arguments are:
     * 1. option name
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_getOption(lua_State * lua)
    {
        int args = lua_gettop(lua);
        if (args != 1)
        {
            const std::string msg = "setOption expects a name as argument.\n";
            lua_pushlstring(lua, msg.c_str(), msg.length());
            return lua_error(lua);
        }
        LuaString s; s.read(lua, 1);

        bool * bval = boolFromName(s.characters);
        if (bval != nullptr)
        {
            lua_pushboolean(lua, *bval);
            return 1;
        }

        float * fval = floatFromName(s.characters);
        if (fval != nullptr)
        {
            lua_pushnumber(lua, *fval);
            return 1;
        }

        uint8_t * i8val = i8FromName(s.characters);
        if (i8val != nullptr)
        {
            lua_pushinteger(lua, *i8val);
            return 1;
        }

        uint64_t * i64val = i64FromName(s.characters);
        if (i64val != nullptr)
        {
            lua_pushinteger(lua, *i64val);
            return 1;
        }

        std::string * sval = stringFromName(s.characters);
        if (sval != nullptr)
        {
            lua_pushstring(lua, sval->c_str());
            return 1;
        }
        const std::string msg = "getOption, no option " + s.characters + ".\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    /**
     * @brief Determine if help or licenses should be printed.
     *
     * @param arg the argument to check.
     * @remark Calls std::exit(EXIT_SUCCESS) after displaying information.
     */
    void checkSpecial(std::string arg)
    {
        if (arg == "-h" || arg == "-help" || arg == "--h" || arg == "--help")
        {
            help();
            std::exit(EXIT_SUCCESS);
        }
        if
        (
            arg == "-l" || arg == "-v" || arg == "-license" || arg == "-version" ||
            arg == "--l" || arg == "--v" || arg == "--license" || arg == "--version"
        )
        {
            welcome();
            licenses();
            std::exit(EXIT_SUCCESS);
        }
        if (arg == "-gpl" || arg == "--gpl")
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
          << "\nUsage:\n\n sfoav structure_path [options...]"
          << "\n\nOptions:\n\n"
          << " -[] takes a value, --[] is a flag.\n\n"
          << sidebyside(argumentHelp(resolution), argumentHelp(msaa), 42)
          << "\n"
          << sidebyside(argumentHelp(script), argumentHelp(darkTheme), 42)
          << "\n"
          << sidebyside(argumentHelp(bondSize), argumentHelp(bondCutoff), 42)
          << "\n"
          << sidebyside(argumentHelp(atomSize), argumentHelp(sizeByMass), 42)
          << "\n"
          << sidebyside(argumentHelp(bondFocus), argumentHelp(focus), 42)
          << "\n"
          << sidebyside(argumentHelp(play), argumentHelp(speed), 42)
          << "\n"
          << sidebyside(argumentHelp(colourmap), argumentHelp(atomColours), 42)
          << "\n"
          << sidebyside(argumentHelp(mesh), argumentHelp(meshes), 42)
          << "\n"
          << sidebyside(argumentHelp(hideAtoms), argumentHelp(deemphasisAlpha), 42)
          << "\n"
          << sidebyside(argumentHelp(showAxes), argumentHelp(showCell), 42)
          << "\n"
          << sidebyside(argumentHelp(atomClipCorrection), argumentHelp(bondClipCorrection), 42)
          << "\n"
          << sidebyside(argumentHelp(globalAtomAlpha), argumentHelp(globalBondAlpha), 42)
          << "\n"
          << sidebyside(argumentHelp(noCentering), argumentHelp(noTransparencySorting), 42)
          << "\n"
          << sidebyside(argumentHelp(levelOfDetail), argumentHelp(hideInfoText), 42)
          << "\n"
          << sidebyside(argumentHelp(videoName), argumentHelp(fieldOfView), 42)
          << "\n"
          << sidebyside(argumentHelp(cameraRotateSpeed), argumentHelp(cameraInclineSpeed), 42)
          << "\n"
          << sidebyside(argumentHelp(cameraPanSpeed), argumentHelp(cameraZoomSpeed), 42)
          << "\n"
          #ifdef WITH_FFMPEG
          << "\n FFMPEG recording options:\n\n"
          << "   See here for advice https://trac.ffmpeg.org/wiki/Encode/H.264\n\n"
          << sidebyside(argumentHelp(codec), argumentHelp(qp), 42)
          << "\n"
          << sidebyside(argumentHelp(crf), argumentHelp(cq), 42)
          << "\n"
          << sidebyside(argumentHelp(maxBFrames), argumentHelp(gopSize), 42)
          << "\n"
          << sidebyside(argumentHelp(preset), argumentHelp(profile), 42)
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

Repository: github.com/JerboaBurrow/SimpleFastOpenAtomicVisualiser
Bug reports: github.com/JerboaBurrow/SimpleFastOpenAtomicVisualiser/issues)";

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

STBIMAGEWRITE
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

private:

    /**
     * @brief true if a string starts with [prefix].
     *
     * @param s string to check
     * @param prefix the prefix
     * @return true s is "[prefix][...]"
     * @return false s is not "[prefix][...]"
     */
    bool startsWith(std::string s, std::string prefix)
    {
        return s.rfind(prefix, 0) == 0;
    }

    /**
     * @brief If a CLI option is a flag (without a value).
     *
     * @param s the CLI option.
     * @return true the option is a flag (starts with "--").
     * @return false the option is not a flag.
     */
    bool isFlag(std::string s) { return startsWith(s, "--"); }

    /**
     * @brief If a const char * is an option's value.
     *
     * @param s the CLI argument.
     * @return true s does not start with "-" or "--".
     * @return false s starts with "-" or "--".
     */
    bool isValue(std::string s) { return !startsWith(s, "--") && !startsWith(s, "-"); }

    std::string stripPrefix(const char * option)
    {
        std::string s = option;
        if (s.size() > 0 && s[0] == '-') { s.erase(0, 1); }
        if (s.size() > 0 && s[0] == '-') { s.erase(0, 1); }
        return s;
    }

    template <class T>
    void extractArgument
    (
        Argument<T> & arg,
        std::map<std::string, const char *> & arguments
    )
    {
        auto loc = arguments.find(arg.name);
        if (loc != arguments.cend())
        {
            getArgument<T>(arg, arguments[arg.name]);
            arguments.erase(loc);
        }
    }

    void extractFlag
    (
        Argument<bool> & flag,
        std::vector<std::string> & flags
    )
    {
        auto loc = std::find(flags.begin(), flags.end(), flag.name);
        if (loc != flags.cend())
        {
            flag.value = true;
            flags.erase(loc);
        }
        else
        {
            flag.value = false;
        }
    }

    bool * boolFromName(const std::string & name)
    {
        std::map<std::string, Argument<bool> &> values
        {
            {meshes.name, meshes},
            {hideAtoms.name, hideAtoms},
            {showAxes.name, showAxes},
            {showCell.name, showCell},
            {hideInfoText.name, hideInfoText},
            {play.name, play},
            {noCentering.name, noCentering},
            {darkTheme.name, darkTheme},
            {noTransparencySorting.name, noTransparencySorting},
            {sizeByMass.name, sizeByMass}
        };

        if (values.find(name) != values.cend())
        {
            return &values.at(name).value;
        }
        return nullptr;
    }

    float * floatFromName(const std::string & name)
    {
        std::map<std::string, Argument<float> &> values
        {
            {bondCutoff.name, bondCutoff},
            {bondSize.name, bondSize},
            {atomSize.name, atomSize},
            {globalAtomAlpha.name, globalAtomAlpha},
            {globalBondAlpha.name, globalBondAlpha},
            {deemphasisAlpha.name, deemphasisAlpha},
            {atomClipCorrection.name, atomClipCorrection},
            {bondClipCorrection.name, bondClipCorrection}
        };

        if (values.find(name) != values.cend())
        {
            return &values.at(name).value;
        }
        return nullptr;
    }

    uint8_t * i8FromName(const std::string & name)
    {
        #ifdef WITH_FFMPEG
        std::map<std::string, Argument<uint8_t> &> values
        {
            {levelOfDetail.name, levelOfDetail},
            {msaa.name, msaa},
            {speed.name, speed},
            {crf.name, crf},
            {qp.name, qp},
            {cq.name, cq}
        };
        #else
        std::map<std::string, Argument<uint8_t> &> values
        {
            {levelOfDetail.name, levelOfDetail},
            {msaa.name, msaa},
            {speed.name, speed}
        };
        #endif

        if (values.find(name) != values.cend())
        {
            return &values.at(name).value;
        }
        return nullptr;
    }

    uint64_t * i64FromName(const std::string & name)
    {
        #ifdef WITH_FFMPEG
        std::map<std::string, Argument<uint64_t> &> values
        {
            {bondFocus.name, bondFocus},
            {focus.name, focus},
            {maxBFrames.name, maxBFrames},
            {gopSize.name, gopSize}
        };
        #else
        std::map<std::string, Argument<uint64_t> &> values
        {
            {bondFocus.name, bondFocus},
            {focus.name, focus}
        };
        #endif

        if (values.find(name) != values.cend())
        {
            return &values.at(name).value;
        }
        return nullptr;
    }

    std::string * stringFromName(const std::string & name)
    {
        #ifdef WITH_FFMPEG
        std::map<std::string, Argument<std::string> &> values
        {
            {preset.name, preset},
            {codec.name, codec},
            {profile.name, profile},
            {videoName.name, videoName}
        };
        if (values.find(name) != values.cend())
        {
            return &values.at(name).value;
        }
        #else
        if (name == videoName.name) { return &videoName.value; }
        #endif
        return nullptr;
    }
};

#endif /* COMMANDLINE_H */
