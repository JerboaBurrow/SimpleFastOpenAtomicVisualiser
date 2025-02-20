#ifndef LUAVEC
#define LUAVEC

#include <lua.h>
#include <vector>

/**
 * @brief Interop for a variable size numeric array (vector) in Lua.
 *
 */
struct LuaVec
{
    /**
     * @brief Construct an empty vector.
     *
     */
    LuaVec(){}

    /**
     * @brief Read the vector from stack index index.
     *
     * @param lua the Lua context.
     * @param index the stack index.
     */
    void read(lua_State * lua, int index)
    {
        elements = getNumericLuaTable(lua, index);
    }

    /**
     * @brief Read the vector from a table field.
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
     * @brief Read the vector from a global table field.
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

    size_t size() const { return elements.size(); }

    operator std::vector<double>() { return elements; }

    std::vector<double> elements;
};

#endif /* LUAVEC */
