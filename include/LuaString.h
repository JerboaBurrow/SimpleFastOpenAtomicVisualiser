#ifndef LUASTRING
#define LUASTRING

#include <lua.h>
#include <string>

/**
 * @brief Interop for a string in Lua.
 *
 */
struct LuaString
{
    /**
     * @brief Construct the string "".
     *
     */
    LuaString()
    {
        characters = "";
    }

    /**
     * @brief Read the string from stack index index.
     *
     * @param lua the Lua context.
     * @param index the stack index.
     */
    void read(lua_State * lua, int index)
    {
        characters = lua_tostring(lua, index);
    }

    /**
     * @brief Read the string from a table field.
     *
     * @param lua the Lua context.
     * @param name the field name.
     * @return true succesful read.
     * @return false unsuccesful read.
     */
    bool readField(lua_State * lua, const char * name)
    {
        int returnType = lua_getfield(lua, 1, name);

        if (returnType == LUA_TSTRING)
        {
            read(lua, 2);
            lua_pop(lua,1);
            return true;
        }
        else
        {
            lua_pop(lua,1);
            return false;
        }
    }

    /**
     * @brief Read the string from a global table field.
     *
     * @param lua the Lua context.
     * @param name the field name.
     * @return true succesful read.
     * @return false unsuccesful read.
     */
    bool readGlobal(lua_State * lua, const char * name)
    {
        int returnType = lua_getglobal(lua, name);

        if (returnType == LUA_TSTRING)
        {
            read(lua, -1);
            lua_pop(lua,1);
            return true;
        }
        else
        {
            lua_pop(lua,1);
            return false;
        }
    }


    bool operator ==(const std::string & rhs){ return characters == rhs; }
    operator std::string() { return characters; }

    std::string characters;
};

#endif /* LUASTRING */
