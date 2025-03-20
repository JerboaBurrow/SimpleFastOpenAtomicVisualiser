#ifndef CONSOLE_H
#define CONSOLE_H

#include <memory>
#include <vector>
#include <chrono>
#include <string>
#include <sstream>

#include <jLog/jLog.h>

#include <lua.h>
#include <LuaNumber.h>
#include <LuaString.h>
#include <LuaTable.h>
#include <LuaArray.h>
#include <LuaVec.h>
#include <LuaBool.h>
#include <visualisationState.h>
#include <commandLine.h>
#include <camera.h>

/**
 * @brief Store for lua global state.
 *  For the console to have access to these classes they must
 *  be set into LuaExtraSpace and set via Console::luaStore.
 */
struct LuaExtraSpace
{
    VisualisationState * visualisationState;
    CommandLine * options;
    Camera * camera;
    bool * exit;
};

/**
 * @brief A Lua binding in VisualisationState;
 *
 */
typedef int (VisualisationState::*VisualisationStateMember)(lua_State * lua);

/**
 * @brief Dispath to a Lua binding of VisualisationState.
 *
 * @see LuaExtraSpace.
 * @tparam function the Lua binding to dispatch.
 * @param lua the Lua context.
 * @return int the return code.
 */
template <VisualisationStateMember function>
int dispatchVisualisationState(lua_State * lua)
{
    LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
    VisualisationState * ptr = store->visualisationState;
    return ((*ptr).*function)(lua);
}

/**
 * @brief A Lua binding in Camera;
 *
 */
typedef int (Camera::*CameraMember)(lua_State * lua);

/**
 * @brief Dispath to a Lua binding of Camera.
 *
 * @see LuaExtraSpace.
 * @tparam function the Lua binding to dispatch.
 * @param lua the Lua context.
 * @return int the return code.
 */
template <CameraMember function>
int dispatchCamera(lua_State * lua)
{
    LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
    Camera * ptr = store->camera;
    return ((*ptr).*function)(lua);
}

/**
 * @brief Start video recording (if not already recording).
 *
 * @param lua the Lua context.
 * @return int the return code.
 */
int lua_startRecord(lua_State * lua)
{
    LuaExtraSpace * extraSpace = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
    if (!extraSpace->visualisationState->recording)
    {
        extraSpace->visualisationState->toggleRecord(*extraSpace->options);
    }
    return 0;
}

/**
 * @brief Stop video recording (if already recording).
 *
 * @param lua the Lua context.
 * @return int the return code.
 */
int lua_stopRecord(lua_State * lua)
{
    LuaExtraSpace * extraSpace = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
    if (extraSpace->visualisationState->recording)
    {
        extraSpace->visualisationState->toggleRecord(*extraSpace->options);
    }
    return 0;
}


/**
 * @brief Start playing frames.
 *
 * @param lua the Lua context.
 * @return int the return code.
 */
int lua_play(lua_State * lua)
{
    LuaExtraSpace * extraSpace = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
    extraSpace->options->play.value = true;
    return 0;
}

/**
 * @brief Exit SFOAV.
 *
 * @param lua the Lua context.
 * @return int the return code.
 */
int lua_exit(lua_State * lua)
{
    LuaExtraSpace * extraSpace = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
    *(extraSpace->exit) = true;
    return 0;
}

/**
 * @brief Stop playing frames.
 *
 * @param lua the Lua context.
 * @return int the return code.
 */
int lua_pause(lua_State * lua)
{
    LuaExtraSpace * extraSpace = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
    extraSpace->options->play.value = false;
    return 0;
}

/**
 * @brief Lua console.
 */
class Console
{
public:

    /**
     * @brief Construct a new Console with a jLog::Log.
     *
     * @param l jLog::Log outputting Lua's messages.
     */
    Console
    (
        jLog::Log & l,
        VisualisationState * visualisationState,
        CommandLine * options,
        Camera * camera
    )
    : lastCommandOrProgram(""), lastStatus(false), log(l)
    {
        lua = luaL_newstate();
        luaL_openlibs(lua);
        luaL_requiref(lua,"sfoav",load_sfoavLib,1);
        extraSpace.visualisationState = visualisationState;
        extraSpace.options = options;
        extraSpace.camera = camera;
        extraSpace.exit = &exit;
        *static_cast<LuaExtraSpace**>(lua_getextraspace(lua)) = &extraSpace;
    }

    ~Console(){ lua_close(lua); }

    /**
     * @brief Attempt to run a Lua script from a file on disc.
     *
     * @param file Lua script location.
     * @return true Error occured.
     * @return false OK.
     */
    bool runFile(std::string file)
    {
        if (luaIsOk())
        {
            lastCommandOrProgram = file;
            lastStatus = luaL_loadfile(lua, file.c_str());
            int epos = lua_gettop(lua);
            lua_pushcfunction(lua, traceback);
            lua_insert(lua, epos);
            lastStatus = lastStatus || lua_pcall(lua, 0, LUA_MULTRET, epos);
            lua_remove(lua, epos);
            return handleErrors();
        }
        return false;
    }

    /**
     * @brief Attempt to run a Lua script from std::string.
     *
     * @param file Lua script.
     * @return true Error occured.
     * @return false OK.
     */
    bool runString(std::string program)
    {
        if (luaIsOk())
        {   lastCommandOrProgram = program;
            lastStatus = luaL_dostring(lua,program.c_str());
            return handleErrors();
        }
        return false;
    }

    bool luaIsOk(){ return lua_status(lua) == LUA_OK ? true : false; }

    /**
     * @brief Convert Lua's status to a std::string
     *
     * @return std::string Lua status name.
     */
    std::string luaStatus()
    {
        int s = lua_status(lua);

        if (s == LUA_OK){return "LUA_OK";}

        std::string status = lastCommandOrProgram + " | ";

        switch(s)
        {
            case LUA_YIELD:
                status += "LUA_YIELD";
                break;
            case LUA_ERRRUN:
                status += "LUA_ERRRUN";
                break;
            case LUA_ERRSYNTAX:
                status += "LUA_ERRSYNTAX";
                break;
            case LUA_ERRMEM:
                status += "LUA_ERRMEM";
                break;
            case LUA_ERRERR:
                status += "LUA_ERRERR";
                break;
            default:
                status += "LUA_STATUS_UNKOWN";
                break;
        }

        return status;
    }

    template <class T>
    T getGlobal(const char * n)
    {
        T value;
        value.readGlobal(lua, n);
        return value;
    }

    /**
     * @brief If sfoav.exit() has been called.
     *
     * @return true exit has been requested in Lua.
     * @return false exit has not been requested in Lua.
     */
    bool exitCalled() const { return exit; }

private:

    lua_State * lua;
    LuaExtraSpace extraSpace;

    std::string lastCommandOrProgram;
    std::stringstream input;
    static std::string stackTrace;
    bool lastStatus;
    bool exit = false;

    jLog::Log & log;

    static int traceback(lua_State * lua) {
        if (lua_isstring(lua, -1))
        {
            stackTrace = lua_tostring(lua, -1);
            lua_pop(lua, 1);
        }
        luaL_traceback(lua, lua, NULL, 1);
        stackTrace += std::string("\n") + lua_tostring(lua, -1);
        lua_pop(lua, 1);
        return 0;
    }

    bool handleErrors()
    {
        if (lastStatus)
        {
            std::string msg = "Exited with error running "+lastCommandOrProgram+"\n";
            msg += stackTrace;
            jLog::ERR(jLog::ERRORCODE::LUA_ERROR, msg) >> log;
            return true;
        }
        else
        {
            return false;
        }
    }

    static int load_sfoavLib(lua_State * lua)
    {
        luaL_Reg sfoavLib[21] =
        {
            {"setAtomColour", &dispatchVisualisationState<&VisualisationState::lua_setAtomColour>},
            {"getAtomColour", &dispatchVisualisationState<&VisualisationState::lua_getAtomColour>},
            {"bond", &dispatchVisualisationState<&VisualisationState::lua_bond>},
            {"unbond", &dispatchVisualisationState<&VisualisationState::lua_unbond>},
            {"getAtomsBonds", &dispatchVisualisationState<&VisualisationState::lua_getAtomsBonds>},
            {"getAtom", &dispatchVisualisationState<&VisualisationState::lua_getAtom>},
            {"atomCount", &dispatchVisualisationState<&VisualisationState::lua_atomCount>},
            {"getAtomsNeighbours", &dispatchVisualisationState<&VisualisationState::lua_getAtomsNeighbours>},
            {"setText", &dispatchVisualisationState<&VisualisationState::lua_setText>},
            {"getFrame", &dispatchVisualisationState<&VisualisationState::lua_getFrame>},
            {"startRecording", &lua_startRecord},
            {"stopRecording", &lua_stopRecord},
            {"play", &lua_play},
            {"pause", &lua_pause},
            {"cameraPosition", &dispatchCamera<&Camera::lua_cameraPosition>},
            {"setCameraPosition", &dispatchCamera<&Camera::lua_setCameraPosition>},
            {"rotateCamera", &dispatchCamera<&Camera::lua_rotateCamera>},
            {"zoomCamera", &dispatchCamera<&Camera::lua_zoomCamera>},
            {"inclineCamera", &dispatchCamera<&Camera::lua_inclineCamera>},
            {"exit", &lua_exit},
            {NULL, NULL}
        };

        luaL_newlib(lua,sfoavLib);
        return 1;
    }
};

#endif /* CONSOLE_H */
