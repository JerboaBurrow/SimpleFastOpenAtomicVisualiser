#ifndef LUABOOL
#define LUABOOL

#include <lua.h>

/**
 * @brief Interop for booleans Lua.
 *
 */
struct LuaBool
{
    /**
     * @brief Construct a false.
     *
     */
    LuaBool()
    {
        bit = false;
    }

    /**
     * @brief Read the bool from stack index index.
     *
     * @param lua the Lua context.
     * @param index the stack index.
     */
    void read(lua_State * lua, int index)
    {
        bit = lua_toboolean(lua, index);
    }

    /**
     * @brief Read the bool from a table field.
     *
     * @param lua the Lua context.
     * @param name the field name.
     * @return true succesful read.
     * @return false unsuccesful read.
     */
    bool readField(lua_State * lua, const char * name)
    {
        int returnType = lua_getfield(lua, 1, name);

        if (returnType == LUA_TBOOLEAN)
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
     * @brief Read the bool from a global table field.
     *
     * @param lua the Lua context.
     * @param name the field name.
     * @return true succesful read.
     * @return false unsuccesful read.
     */
    bool readGlobal(lua_State * lua, const char * name)
    {
        int returnType = lua_getglobal(lua, name);

        if (returnType == LUA_TBOOLEAN)
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


    bool operator ==(const bool & rhs){ return bit == rhs; }
    operator bool() { return bit; }
    bool bit;
};


#endif /* LUABOOL */
