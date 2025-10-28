#ifndef LUA_CAMERA_H
#define LUA_CAMERA_H

/**
 * @brief Get the Camera position.
 *
 * @remark Lua arguments are:
 * 1. Whether to return in spherical coordinates.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int Camera::lua_cameraPosition(lua_State * lua)
{
    if (lua_gettop(lua) >= 1 && lua_isstring(lua, 1) && std::string(lua_tostring(lua, 1)) == "help")
    {
        lua_writestring("cameraPosition:\n  Arguments:\n    spherical [bool, optional]\n  Get the position of the camera.\n  By default in cartesian form.\n");
        return 0;
    };
    int args = lua_gettop(lua);
    bool spherical = false;
    if (args == 1)
    {
        LuaBool b;
        b.read(lua, 1);
        spherical = b.bit;
    }

    glm::vec3 pos = position(spherical);

    lua_pushnumber(lua, pos.x);
    lua_pushnumber(lua, pos.y);
    lua_pushnumber(lua, pos.z);

    return 3;
}

/**
 * @brief Set Camera position.
 *
 * @remark Lua arguments are:
 * 1. r
 * 2. theta
 * 3. phi
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int Camera::lua_setCameraPosition(lua_State * lua)
{
    if (lua_gettop(lua) >= 1 && lua_isstring(lua, 1) && std::string(lua_tostring(lua, 1)) == "help")
    {
        lua_writestring("setCameraPosition:\n  Arguments:\n    x [number],\b    y [number],\n    z[number]\n  Set the position of the camera.\n");
        return 0;
    };
    int args = lua_gettop(lua);
    if (args != 3)
    {
        const std::string msg = "setCameraPosition expects r, theta, and phi as arguments.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaNumber r, theta, phi;
    r.read(lua, 1);
    theta.read(lua, 2);
    phi.read(lua, 3);
    setPosition(glm::vec3(r.n, theta.n, phi.n));

    return 0;
}

/**
 * @brief Rotate the Camera.
 *
 * @remark Lua arguments are:
 * 1. The azimuthal increment.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int Camera::lua_rotateCamera(lua_State * lua)
{
    if (lua_gettop(lua) >= 1 && lua_isstring(lua, 1) && std::string(lua_tostring(lua, 1)) == "help")
    {
        lua_writestring("rotateCamera:\n  Arguments:\n    phi [number]\n  Rotate the camera by phi radians.\n");
        return 0;
    };
    int args = lua_gettop(lua);
    if (args != 1)
    {
        const std::string msg = "rotateCamera expects a number as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaNumber dphi;
    dphi.read(lua, 1);
    rotate(-dphi.n);

    return 0;
}

/**
 * @brief Zoom the Camera.
 *
 * @remark Lua arguments are:
 * 1. The zoom increment.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int Camera::lua_zoomCamera(lua_State * lua)
{
    if (lua_gettop(lua) >= 1 && lua_isstring(lua, 1) && std::string(lua_tostring(lua, 1)) == "help")
    {
        lua_writestring("zoomCamera:\n  Arguments:\n    r [number]\n  Zoom the camera.\n");
        return 0;
    };
    int args = lua_gettop(lua);
    if (args != 1)
    {
        const std::string msg = "zoomCamera expects a number as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaNumber dr;
    dr.read(lua, 1);
    zoom(-dr.n);

    return 0;
}

/**
 * @brief Incline the camera.
 *
 * @remark Lua arguments are:
 * 1. The polar increment.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int Camera::lua_inclineCamera(lua_State * lua)
{
    if (lua_gettop(lua) >= 1 && lua_isstring(lua, 1) && std::string(lua_tostring(lua, 1)) == "help")
    {
        lua_writestring("inclineCamera:\n  Arguments:\n    theta [number]\n  Incline the camera by theta radians.\n");
        return 0;
    };
    int args = lua_gettop(lua);
    if (args != 1)
    {
        const std::string msg = "inclineCamera expects a number as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaNumber dtheta;
    dtheta.read(lua, 1);
    incline(dtheta.n);

    return 0;
}

/**
 * @brief Set the field of view.
 *
 * @remark Lua arguments are:
 * 1. Field of view in degrees.
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int Camera::lua_setCameraFieldOfView(lua_State * lua)
{
    if (lua_gettop(lua) >= 1 && lua_isstring(lua, 1) && std::string(lua_tostring(lua, 1)) == "help")
    {
        lua_writestring("setCameraFieldOfView:\n  Arguments:\n    fov [number]\n  Set the camera's fov in degrees.\n");
        return 0;
    };
    int args = lua_gettop(lua);
    if (args != 1)
    {
        const std::string msg = "setCameraFieldOfView expects a number as argument.\n";
        lua_pushlstring(lua, msg.c_str(), msg.length());
        return lua_error(lua);
    }

    LuaNumber fov;
    fov.read(lua, 1);
    fieldOfView = fov.n;
    reset();

    return 0;
}

/**
 * @brief Get the field of view.
 *
 * @remark Lua arguments are:
 * @param lua the Lua context.
 * @return int the return code.
 */
inline int Camera::lua_getCameraFieldOfView(lua_State * lua)
{
    if (lua_gettop(lua) >= 1 && lua_isstring(lua, 1) && std::string(lua_tostring(lua, 1)) == "help")
    {
        lua_writestring("getCameraFieldOfView:\n  Arguments: none\n  Get the camera's fov in degrees.\n");
        return 0;
    };
    lua_pushnumber(lua, fieldOfView);
    return 1;
}

#endif /* LUA_CAMERA_H */
