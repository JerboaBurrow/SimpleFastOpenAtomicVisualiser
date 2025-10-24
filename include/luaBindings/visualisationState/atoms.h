#ifndef ATOMS_H
#define ATOMS_H

#include <string>

#include <atom.h>
#include <element.h>
#include <visualisationState.h>
#include <LuaNumber.h>

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
    elementsUpdated = true;

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

/**
 * @brief Lua binding to get the Atom count.
 *
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int VisualisationState::lua_atomCount(lua_State * lua)
{
    lua_pushinteger(lua, atomCount);
    return 1;
}

/**
 * @brief Lua binding to get the neighbours of an Atom to a cutoff.
 *
 * @remark Lua arguments are:
 * 1. The Atom index.
 * 2. The cutoff distance.
 * 3. [Optional] whether to use nearest images.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int VisualisationState::lua_getAtomsNeighbours(lua_State * lua)
{
    int args = lua_gettop(lua);
    if (args < 2)
    {
        const std::string msg = "getAtomsNeighbours expects an atom index and cutoff distance as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaNumber li, cutoff;
    li.read(lua, 1);
    cutoff.read(lua, 2);

    bool nearestImage = true;
    if (args == 3)
    {
        LuaBool b;
        b.read(lua, 3);
        nearestImage = b.bit;
    }

    uint64_t i = li.n;

    if (i >= atomCount)
    {
        const std::string msg = "getAtom atom index larger than atom count.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }
    Neighbours neighbourList(atoms, 2.0f*cutoff.n);
    auto n = neighbourList.neighbours(atoms, atoms[i].position, cutoff, false, nearestImage);

    i = 1;
    lua_createtable(lua, n.size(), 0);
    for (auto jd : n)
    {
        lua_createtable(lua, 2, 0);
            lua_pushnumber(lua, jd.first);
            lua_setfield(lua, -2, "index");
            lua_pushnumber(lua, jd.second);
            lua_setfield(lua, -2, "distance");
        lua_rawseti(lua, -2, i);
        i++;
    }
    return 1;
}

/**
 * @brief Lua binding to get an Atom.
 *
 * @remark Lua arguments are:
 * 1. The Atom index.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int VisualisationState::lua_getAtom(lua_State * lua)
{
    int args = lua_gettop(lua);
    if (args != 1)
    {
        const std::string msg = "getAtom expects an atom index as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaNumber li;
    li.read(lua, 1);

    uint64_t i = li.n;

    if (i >= atomCount)
    {
        const std::string msg = "getAtom atom index larger than atom count.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    Atom & atom = atoms[i];
    std::string element = STRING_FROM_ELEMENT.at(atom.symbol);

    lua_createtable(lua, 6, 0);
        lua_pushstring(lua, element.c_str());
        lua_setfield(lua, -2, "element");
        lua_pushnumber(lua, atom.scale);
        lua_setfield(lua, -2, "radius");
        lua_createtable(lua, 3, 0);
            lua_pushnumber(lua, atom.position.x);
            lua_setfield(lua, -2, "x");
            lua_pushnumber(lua, atom.position.y);
            lua_setfield(lua, -2, "y");
            lua_pushnumber(lua, atom.position.z);
            lua_setfield(lua, -2, "z");
        lua_setfield(lua, -2, "position");
        lua_createtable(lua, 3, 0);
            lua_pushnumber(lua, atom.velocity.x);
            lua_setfield(lua, -2, "x");
            lua_pushnumber(lua, atom.velocity.y);
            lua_setfield(lua, -2, "y");
            lua_pushnumber(lua, atom.velocity.z);
            lua_setfield(lua, -2, "z");
        lua_setfield(lua, -2, "velocity");
        lua_createtable(lua, 3, 0);
            lua_pushnumber(lua, atom.force.x);
            lua_setfield(lua, -2, "x");
            lua_pushnumber(lua, atom.force.y);
            lua_setfield(lua, -2, "y");
            lua_pushnumber(lua, atom.force.z);
            lua_setfield(lua, -2, "z");
        lua_setfield(lua, -2, "force");
        lua_createtable(lua, 4, 0);
            lua_pushnumber(lua, atom.colour.r);
            lua_setfield(lua, -2, "r");
            lua_pushnumber(lua, atom.colour.g);
            lua_setfield(lua, -2, "g");
            lua_pushnumber(lua, atom.colour.b);
            lua_setfield(lua, -2, "b");
            lua_pushnumber(lua, atom.colour.a);
            lua_setfield(lua, -2, "a");
        lua_setfield(lua, -2, "colour");

    return 1;

}

/**
 * @brief Lua binding to get the bonds of an Atom.
 *
 * @remark Lua arguments are:
 * 1. The Atom index.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int VisualisationState::lua_getAtomsBonds(lua_State * lua)
{
    int args = lua_gettop(lua);
    if (args != 1)
    {
        const std::string msg = "getAtomsBonds expects an atom index as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaNumber li;
    li.read(lua, 1);

    uint64_t i = li.n;

    if (i >= atomCount)
    {
        const std::string msg = "getAtomsBonds atom index larger than atom count.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    auto bonded = bonds[i];
    lua_createtable(lua, bonded.size(), 0);
    if (bonded.size() > 0)
    {
        uint64_t i = 1;
        for (auto j : bonded)
        {
            lua_pushinteger(lua, j);
            lua_rawseti(lua, -2, i);
            i++;
        }
    }

    return 1;
}


#endif /* ATOMS_H */
