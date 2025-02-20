#ifndef LUANUMBER_H
#define LUANUMBER_H

#include <lua.h>

/**
 * @brief Interop for a number in Lua.
 *
 */
struct LuaNumber
{
    /**
     * @brief Construct a 0.0
     *
     */
    LuaNumber()
    : n(0.0)
    {}

    /**
     * @brief Read the number from stack index index.
     *
     * @param lua the Lua context.
     * @param index the stack index.
     */
    void read(lua_State * lua, int index)
    {
        n = lua_tonumber(lua, index);
    }

    /**
     * @brief Read the number from a table field.
     *
     * @param lua the Lua context.
     * @param name the field name.
     * @return true succesful read.
     * @return false unsuccesful read.
     */
    bool readField(lua_State * lua, const char * name)
    {
        int returnType = lua_getfield(lua, 1, name);

        if (returnType == LUA_TNUMBER)
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
     * @brief Read the number from a global table field.
     *
     * @param lua the Lua context.
     * @param name the field name.
     * @return true succesful read.
     * @return false unsuccesful read.
     */
    bool readGlobal(lua_State * lua, const char * name)
    {
        int returnType = lua_getglobal(lua, name);

        if (returnType == LUA_TNUMBER)
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


    bool operator ==(const double & rhs){ return n == rhs; }
    operator double() { return n; }
    double n;
};


#endif /* LUANUMBER_H */