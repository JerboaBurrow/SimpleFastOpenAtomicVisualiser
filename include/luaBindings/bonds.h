#ifndef BONDS_H
#define BONDS_H

/**
 * @brief Lua binding to bond 2 Atoms.
 *
 * @remark Lua arguments are:
 * 1. The first Atom index.
 * 2. The second Atom index.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int VisualisationState::lua_bond(lua_State * lua)
{
    int args = lua_gettop(lua);
    if (args != 2)
    {
        const std::string msg = "bond expects two atom indices as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaNumber li, lj;
    li.read(lua, 1);
    lj.read(lua, 2);

    uint64_t i = li.n;
    uint64_t j = lj.n;

    if (i >= atomCount || j >= atomCount)
    {
        const std::string msg = "bond atom index larger than atom count.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    bonds[i].insert(j);
    auto s = bonds[j];
    if (s.find(i) != s.cend())
    {
        bonds[j].erase(i);
    }

    return 0;
}

/**
 * @brief Lua binding to unbond 2 Atoms.
 *
 * @remark Lua arguments are:
 * 1. The first Atom index.
 * 2. The second Atom index.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int VisualisationState::lua_unbond(lua_State * lua)
{
    int args = lua_gettop(lua);
    if (args != 2)
    {
        const std::string msg = "unbond expects two atom indices as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaNumber li, lj;
    li.read(lua, 1);
    lj.read(lua, 2);

    uint64_t i = li.n;
    uint64_t j = lj.n;

    if (i >= atomCount || j >= atomCount)
    {
        const std::string msg = "unbond atom index larger than atom count.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    auto s = bonds[i];
    if (s.find(j) != s.cend())
    {
        bonds[i].erase(j);
    }

    s = bonds[j];
    if (s.find(i) != s.cend())
    {
        bonds[j].erase(i);
    }

    return 0;
}

#endif /* BONDS_H */
