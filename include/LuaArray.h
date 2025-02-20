#ifndef LUAARRAY_H
#define LUAARRAY_H

#include <lua.h>
#include <array>


/**
 * @brief Interop for a fixed size numeric array in Lua.
 *
 * @tparam DIM the array's size.
 */
template <size_t DIM>
struct LuaArray
{
    /**
     * @brief Construct an array of 0s
     *
     */
    LuaArray()
    {
        elements.fill(0.0);
    }

    /**
     * @brief Read the array from stack index index.
     *
     * @param lua the Lua context.
     * @param index the stack index.
     */
    void read(lua_State * lua, int index)
    {
        std::vector data = getNumericLuaTable(lua, index);
        if (data.size() != DIM)
        {
            std::string err = "Wrong number of elements to LuaArray<"+std::to_string(DIM)+">, " + std::to_string(data.size());
            lua_pushliteral(lua, "Wrong number of elements to LuaArray");
            lua_error(lua);
            throw std::runtime_error(err);
        }
        else
        {
            // gauranteed the correct size
            std::copy_n(data.begin(), DIM, elements.begin());
        }
    }

    /**
     * @brief Read the array from a table field.
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
     * @brief Read the array from a global table field.
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

    double & operator [] (size_t i){ return elements[i]; }

    operator std::array<double, DIM>() { return elements; }

    std::array<double, DIM> elements;
};

#endif /* LUAARRAY_H */
