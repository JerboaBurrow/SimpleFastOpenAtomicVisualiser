#ifndef LUATABLE
#define LUATABLE

#include <lua.h>

#include <vector>

/**
 * @brief Interop for a single-type Lua table.
 *
 * @tparam LUA_TYPE the Lua type (which may be LuaTable<S>).
 */
template <class LUA_TYPE>
struct LuaTable
{
    /**
     * @brief Construct an default table.
     *
     */
    LuaTable(){}

    /**
     * @brief Read the table from stack index index.
     *
     * @param lua the Lua context.
     * @param index the stack index.
     */
    void read(lua_State * lua, int index)
    {
        unsigned length = lua_rawlen(lua,index);
        data.resize(length);

        for (unsigned i = 1; i <= length; i++)
        {
            lua_pushnumber(lua, i);
            lua_gettable(lua, index);
            data[i-1].read(lua, lua_gettop(lua));

            lua_pop(lua, 1);
        }
    }

    /**
     * @brief Read the table from a table field.
     *
     * @param lua the Lua context.
     * @param name the field name.
     * @return true succesful read.
     * @return false unsuccesful read.
     */
    bool readField(lua_State * lua, const char * name)
    {
        int returnType = lua_getfield(lua, 1, name);

        if (returnType == LUA_TTABLE)
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
     * @brief Read the table from a global table field.
     *
     * @param lua the Lua context.
     * @param name the field name.
     * @return true succesful read.
     * @return false unsuccesful read.
     */
    bool readGlobal(lua_State * lua, const char * name)
    {
        int returnType = lua_getglobal(lua, name);

        if (returnType == LUA_TTABLE)
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


    size_t size() const { return data.size(); }

    LUA_TYPE & operator [] (size_t i){ return data[i]; }

    operator std::vector<LUA_TYPE>() { return data; }

    std::vector<LUA_TYPE> data;
};

#endif /* LUATABLE */
