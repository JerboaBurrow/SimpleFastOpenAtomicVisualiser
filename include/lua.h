#ifndef LUA_H
#define LUA_H

#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <algorithm>

extern "C"
{
    #include <lua/src/lua.h>
    #include <lua/src/lauxlib.h>
    #include <lua/src/lualib.h>
}

/**
* @brief Get the a Lua table of doubles.
*
* @param lua the Lua context.
* @param index the stack index of the table.
* @return std::vector<double> the values.
*/
std::vector<double> getNumericLuaTable(lua_State * lua, int index)
{
    unsigned length = lua_rawlen(lua,index);
    std::vector<double> values(length);

    for (unsigned i = 1; i <= length; i++)
    {
        lua_pushnumber(lua,i);
        // replaces number i (key) with the value
        lua_gettable(lua,index);
        values[i-1] = lua_tonumber(lua,-1);
        lua_pop(lua,1);
    }

    return values;
}

/**
* @brief Get the a Lua table of table of doubles.
*
* @param lua the Lua context.
* @param index the stack index of the table.
* @return std::vector<std::vector<double>> the values.
*/
std::vector<std::vector<double>> getLuaTableOfNumericLuaTable(lua_State * lua, int index)
{
    unsigned length = lua_rawlen(lua,index);
    std::vector<std::vector<double>> values;

    for (unsigned i = 1; i <= length; i++)
    {
        lua_pushnumber(lua,i);
        lua_gettable(lua,index);

        values.push_back
        (
            // not -1, since stack will be added to
            // in this call, need absolute position
            getNumericLuaTable(lua, lua_gettop(lua))
        );

        lua_pop(lua,1);
    }
    return values;
}

/**
* @brief Check the number of arguments to the Lua call.
*
* @param lua the Lua context.
* @param expected the expected argument count.
* @param msg the error message to write if failing.
* @return int the Lua return code.
*/
int lua_checkArgumentCount(lua_State * lua, int expected, std::string msg)
{
    int n = lua_gettop(lua);
    if (n != expected)
    {
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }
    return LUA_OK;
}

#endif /* LUA_H */
