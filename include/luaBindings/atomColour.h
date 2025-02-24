#ifndef ATOMCOLOUR_H
#define ATOMCOLOUR_H

#include <visualisationState.h>

/**
 * @brief Lua binding to set an Atoms colour by index.
 *
 * @remark Lua arguments are:
 * 1. The Atom index.
 * 2. The red channel [0, 1].
 * 3. The green channel [0, 1].
 * 4. The blue channel [0, 1].
 * 5. [Optional] The alpha channel [0, 1] defaults to 1.
 * Call as sfoav.setAtomColour(5, 1.0, 0.5, 0.5, 1.0)
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int VisualisationState::lua_setAtomColour(lua_State * lua)
{
    int args = lua_gettop(lua);
    if (args < 4 || args > 5)
    {
        const std::string msg = "setAtomColour expects an atom index and RGB or RGBA arguments.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }
    LuaNumber lua_index, lua_r, lua_g, lua_b, lua_a;
    uint64_t index;
    float r, g, b, a;
    a = 1.0f;

    lua_index.read(lua, 1);
    index = uint64_t(lua_index.n);

    if (index >= atomCount)
    {
        const std::string msg = "setAtomColour atom index larger than atom count.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }
    lua_r.read(lua, 2);
    r = std::clamp(float(lua_r.n), 0.0f, 1.0f);
    lua_g.read(lua, 3);
    g = std::clamp(float(lua_g.n), 0.0f, 1.0f);
    lua_b.read(lua, 4);
    b = std::clamp(float(lua_b.n), 0.0f, 1.0f);
    if (args == 5)
    {
        lua_a.read(lua, 5);
        a = std::clamp(float(lua_a.n), 0.0f, 1.0f);
    }
    atomColourOverrides[index] = glm::vec4(r, g, b, a);

    return 0;
}

/**
 * @brief Lua binding to get a Atom's colour by index.
 *
 * @remark Lua arguments are:
 * 1. The Atom index.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int VisualisationState::lua_getAtomColour(lua_State * lua)
{
    int args = lua_gettop(lua);
    if (args != 1)
    {
        const std::string msg = "getAtomColour expects an atom index as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }
    LuaNumber lua_index;
    uint64_t index;

    lua_index.read(lua, 1);
    index = uint64_t(lua_index.n);

    if (index >= atomCount)
    {
        const std::string msg = "getAtomColour atom index larger than atom count.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    glm::vec4 colour = atoms[index].colour;
    if (atomColourOverrides.find(index) != atomColourOverrides.cend())
    {
        colour = atomColourOverrides[index];
    }
    colour.a = atomEmphasisOverrides[index];

    lua_pushnumber(lua, colour.r);
    lua_pushnumber(lua, colour.g);
    lua_pushnumber(lua, colour.b);
    lua_pushnumber(lua, colour.a);

    return 4;
}

#endif /* ATOMCOLOUR_H */
