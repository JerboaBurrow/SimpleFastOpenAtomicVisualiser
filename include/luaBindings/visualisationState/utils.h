#ifndef UTILS_H
#define UTILS_H

/**
 * @brief Set the on screen text.
 *
 * @remark Lua arguments are:
 * 1. The text string.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int VisualisationState::lua_setText(lua_State * lua)
{
    int args = lua_gettop(lua);
    if (args != 1)
    {
        const std::string msg = "setText expects a string as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaString s;
    s.read(lua, 1);

    text = s.characters;

    return 0;
}

/**
 * @brief Get the current frame number.
 *
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int VisualisationState::lua_getFrame(lua_State * lua)
{
    lua_pushinteger(lua, frame);
    return 1;
}

#endif /* UTILS_H */
